#define block(r,a,c)({struct{static r _ a{c;}}_;_._;})
#define CALLBACK(func) _w67,block(void,(W67 *ctx,Shader *shader),func)
typedef void (*BLOCK)(W67*,Shader*);

namespace EXT { enum {
        FLOATING_FBO = 1
    };
}

namespace Type { enum {
        Shader       = 1,
        TEXTURE      = 1<<1,
        FBO          = 1<<2,
        FLOATING_FBO = 1<<3
    };
}

class W67 {
         
     private:
        
        #ifdef EMSCRIPTEN
            EMSCRIPTEN_WEBGL_CONTEXT_HANDLE _ctx;
        #endif    
        int _fbo = 0;
        int _shader = 0;
        int _texture = 0;
        
     public:
        
        Shader  *shader[32];
        Texture *fbo[32];
        Texture *texture[32];
        Plane *plane;
        int width;
        int height;
        unsigned char *pixel;

        void clear(int w,int h) {
            glViewport(0,0,w,h);
            glClearColor(0.0,0.0,0.0,1.0);
            glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        }

        W67(int option=0) {
            this->width  = TEX_W;
            this->height = TEX_H;
            this->pixel = (unsigned char *)malloc(TEX_W*TEX_H*4*sizeof(unsigned char *));
            
#ifdef EMSCRIPTEN
            
            EmscriptenWebGLContextAttributes attrs;
            emscripten_webgl_init_context_attributes(&attrs);
            attrs.depth     = 0;
            attrs.stencil   = 0;
            attrs.antialias = 0;
            EM_ASM_ARGS({
                Module.canvas = document.createElement("canvas");
                Module.canvas.id = "stage";
                Module.canvas.width  = $0;
                Module.canvas.height = $1;
                var cs = Module.canvas.style;
                window.onresize = function() {
                    cs.width  = window.innerWidth +"px";
                    cs.height = window.innerHeight+"px";
                };
                window.onresize();
                document.body.appendChild(Module.canvas);    
            },this->width,this->height);
            this->_ctx = emscripten_webgl_create_context(0,&attrs);
            emscripten_webgl_make_context_current(this->_ctx);
            if(option&(EXT::FLOATING_FBO)) {                    
                printf("OES_texture_float\n");
                emscripten_webgl_enable_extension(this->_ctx,"OES_texture_float");
            }
#endif
            this->plane = new Plane();
        }
        
        void cs(int uid ,const char *vs, const char *fs) {                
            if(uid<32) {
                this->shader[uid] = new Shader(vs,fs);                
                this->plane->begin();
                    glVertexAttribPointer(glGetAttribLocation(this->shader[uid]->program,"aP"),3,GL_FLOAT,false,0,0);
                this->plane->end();
            }
        }

        void allocate(int type, int num) {
            if(Type::FBO==type||Type::FLOATING_FBO==type) {
                if((_fbo+num)<32) {                          
                    const char *format = (Type::FLOATING_FBO==type)?R"({
                        "width" :%d,
                        "height":%d,
                        "format":"GL_RGBA",
                        "fbo":true,
                        "type":"GL_FLOAT"
                    })":R"({
                        "width" :%d,
                        "height":%d,
                        "format":"GL_RGBA",
                        "fbo":true
                    })";
                    for(int k=_fbo; k<_fbo+num; k++) {   
                        this->fbo[k] = new Texture(Utils::stringWithFormat(format,TEX_W,TEX_H));
                    }
                    _fbo+=num;
                }
            }
        }
};