#import <stdio.h>
#import <GLES2/gl2.h>
#import <emscripten.h>
#import <emscripten/html5.h>
#import "string"

#define TEX_W 1024
#define TEX_H 1024

class W67;

#import "Utils.h"
#import "Texture.h"
#import "Shader.h"
#import "Plane.h"
#import "W67.h"
#import "ColorMatrix.h"

int timer = 0;
W67 *_w67 = nullptr;
ColorMatrix *cm = new ColorMatrix();

namespace FX { enum {
  COPY = 0,
  CONVOLUTION,
  COLOR_CORRECTION,
  ADD
}; }

namespace TX { enum {
  IMG = 0,
  FBO1,
  FBO2
}; }

void clear(int w=TEX_W,int h=TEX_H) {
  glViewport(0,0,w,h);
  glClearColor(0.0,0.0,0.0,1.0);
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
}

void draw() {
  cm->identity()->saturation(0.96)->contrast(0.18);
  _w67->fbo[TX::FBO1]->bind();
    clear(TEX_W>>1,TEX_H>>1);
    _w67->shader[FX::ADD]->perform(CALLBACK({
      ctx->plane->begin();
        ctx->texture[TX::IMG]->begin(GL_TEXTURE0);
          glUniform1i(glGetUniformLocation(shader->program,"uT"),0);
        ctx->fbo[TX::FBO2]->begin(GL_TEXTURE1);
          glUniform1i(glGetUniformLocation(shader->program,"uU"),1);
          ctx->plane->draw();
        ctx->fbo[TX::FBO2]->end();
        ctx->texture[TX::IMG]->end();
      ctx->plane->end();
    }));
  _w67->fbo[TX::FBO1]->unbind();
  _w67->fbo[TX::FBO2]->bind();
    clear(TEX_W,TEX_H);
    _w67->shader[FX::CONVOLUTION]->perform(CALLBACK({
        ctx->plane->begin();
          ctx->fbo[TX::FBO1]->begin(GL_TEXTURE0);
            glUniform1i(glGetUniformLocation(shader->program,"uT"),0);
            ctx->plane->draw();
          ctx->fbo[TX::FBO1]->end();
        ctx->plane->end();
    }));
  _w67->fbo[TX::FBO2]->unbind();
  _w67->shader[FX::COLOR_CORRECTION]->perform(CALLBACK({    
    clear(TEX_W,TEX_H);
    ctx->plane->begin();
      ctx->fbo[TX::FBO1]->begin(GL_TEXTURE0);
        glUniform1i(glGetUniformLocation(shader->program,"uT"),0);
        glUniformMatrix4fv(glGetUniformLocation(shader->program,"colorMatrix"),1,GL_FALSE,cm->matrix());
        glUniform4fv(glGetUniformLocation(shader->program,"colorOffset"),1,cm->offset());
        ctx->plane->draw();
      ctx->fbo[TX::FBO1]->end();
    ctx->plane->end();
  }));
  
  if(timer<38) {
    timer++;
    if(timer==38) {
      EM_ASM({  
        if(window.indicator) window.indicator.stop();
        Module.canvas.style.opacity = 1.0;
      });
    }
  }
}

extern"C" void setup() {
  _w67 = new W67();
  _w67->texture[TX::IMG] = new Texture(Utils::stringWithFormat(R"({
    "width" :%d,
    "height":%d,
    "format":"GL_RGBA",
    "warp":"GL_MIRRORED_REPEAT",
    "filter":"GL_LINEAR"
  })"
  ,TEX_W,TEX_H));
  const char *format = R"({
    "width" :%d,
    "height":%d,
    "warp":"GL_MIRRORED_REPEAT",
    "format":"GL_RGBA",
    "fbo":true,
    "filter":"GL_LINEAR"
  })";
  _w67->fbo[TX::FBO1]  = new Texture(Utils::stringWithFormat(format,TEX_W>>1,TEX_H>>1)); // 256
  _w67->fbo[TX::FBO2]  = new Texture(Utils::stringWithFormat(format,TEX_W,TEX_H)); // 512
  const char *vs = R"(
    attribute vec3 aP;
    varying vec2 vT;
    void main() {
      gl_Position=vec4(aP,1.0);
      vT=((aP+1.0)*0.5).xy;
    }
  )";
  std::string fs = "precision mediump float;";
  fs+=R"(
    uniform sampler2D uT;
    varying vec2 vT;
    void main(){
      gl_FragColor = texture2D(uT,vT);
    }
  )";
  _w67->cs(FX::COPY,vs,fs.c_str());
  fs = "precision mediump float;";
  fs+=R"(
    uniform sampler2D uT;
    uniform sampler2D uU;
    varying vec2 vT;    
    void main(void){ 
      gl_FragColor = texture2D(uT,vT)+texture2D(uU,vT);
    }
  )";
  _w67->cs(FX::ADD,vs,fs.c_str());
  fs = "precision mediump float;";
  fs+=R"(
    uniform sampler2D uT;
    varying vec2 vT;
    vec4 apply(float x,float y,float z) { return texture2D(uT,vT+vec2(x,y))*z;}
    void main(void){
      gl_FragColor = (
        apply(-0.001,0.001,0.4)+
        apply(0.007,0.001,-0.3)+
        apply(-0.1,0.00,0.4)+
        apply(0.051,0.02,-0.2)+
        apply(0.11,0.12,-0.3)+
        apply(0.341,0.07,0.4)+
        apply(0.121,0.16,0.2)+
        apply(0.221,0.27,-0.2)+
        apply(0.021,0.27,0.1)
      )*2.0-0.2;
    }
  )";
  _w67->cs(FX::CONVOLUTION,vs,fs.c_str());
  fs = "precision mediump float;";
  fs+=R"(
    uniform sampler2D uT;
    varying vec2 vT;
    uniform mat4 colorMatrix;
    uniform vec4 colorOffset;
    void main(void){ 
      gl_FragColor = (texture2D(uT,vT)*colorMatrix+colorOffset); 
    }
  )";
  _w67->cs(FX::COLOR_CORRECTION,vs,fs.c_str());
  _w67->fbo[TX::FBO2]->bind();
    clear(TEX_W,TEX_H);
  _w67->fbo[TX::FBO2]->unbind();
  glBindTexture(GL_TEXTURE_2D,_w67->texture[TX::IMG]->get());
  EM_ASM({   
    var data = new Uint8Array($0*$1*4);
    for(var i=0;i<$1;i++) {
      for(var j=0;j<$0;j++) {
        var addr = (i*1024+j)<<2;
        data[addr+0] = 255*(0.05+(j/$0)*((i/$1))*0.30); 
        data[addr+1] = 255*(0.05+(j/$0)*((i/$1))*0.65); 
        data[addr+2] = 255*(0.05+(j/$0)*((i/$1))*0.40); 
        data[addr+3] = 255;
      }	
    }
    var gl = Module.canvas.getContext("webgl");
    gl.texImage2D(gl.TEXTURE_2D,0,gl.RGBA,1024,1024,0,gl.RGBA,gl.UNSIGNED_BYTE,data);
  },TEX_W,TEX_H);  
  glBindTexture(GL_TEXTURE_2D,0);
  emscripten_set_main_loop(draw,30,false);
}
