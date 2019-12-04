#import <map>
#import "picojson.h"

#define MAP_PAIR(x) {#x,x}

class Texture {
    
    private:
    
        GLuint fbo;
        GLuint depthBuffer;
        GLuint textureID;
    
        int width,height;
        int format;
        int type;
    
        std::map<std::string,int> FORMAT = {
            MAP_PAIR(GL_RGB),
            MAP_PAIR(GL_RGBA)
        };
    
        std::map<std::string,int> FILTER = {
            MAP_PAIR(GL_NEAREST),
            MAP_PAIR(GL_LINEAR)
        };
    
        std::map<std::string,int> WARP = {
            MAP_PAIR(GL_REPEAT),
            //MAP_PAIR(GL_CLAMP),
            MAP_PAIR(GL_CLAMP_TO_EDGE),
            MAP_PAIR(GL_MIRRORED_REPEAT)
            //,MAP_PAIR(GL_CLAMP_TO_BORDER)
        };
    
        std::map<std::string,int> TYPE = {
            MAP_PAIR(GL_UNSIGNED_BYTE),
            MAP_PAIR(GL_FLOAT)
        };
    
    public:
    
        static const int DEFAULT_WIDTH  = 512;
        static const int DEFAULT_HEIGHT = 512;
        
        Texture(const char *jsonstr) {
            picojson::value val;
            std::string err;
            picojson::parse(val,jsonstr,jsonstr+strlen(jsonstr),&err);
            picojson::object json = val.get<picojson::object>();
            bool isFbo = (json["fbo"].is<bool>())?json["fbo"].get<bool>():false;
            this->width  = (json["width"].is<double>())?json["width"].get<double>():DEFAULT_WIDTH;
            this->height = (json["height"].is<double>())?json["height"].get<double>():DEFAULT_HEIGHT;
            this->format = FORMAT["GL_RGB"];
            if(json["format"].is<std::string>()) {
                std::string key = json["format"].get<std::string>();
                if(FORMAT.count(key)) this->format = FORMAT[key];
            }
            int warp = WARP["GL_REPEAT"];
            if(json["warp"].is<std::string>()) {
                std::string key = json["warp"].get<std::string>();
                if(WARP.count(key)) warp = WARP[key];
            }
            int filter = FILTER["GL_NEAREST"];
            if(json["filter"].is<std::string>()) {
                std::string key = json["filter"].get<std::string>();
                if(FILTER.count(key)) filter = FILTER[key];
            }
            this->type = TYPE["GL_UNSIGNED_BYTE"];
            if(json["type"].is<std::string>()) {
                std::string key = json["type"].get<std::string>();
                if(TYPE.count(key)) this->type = TYPE[key];
            }
            glPixelStorei(GL_UNPACK_ALIGNMENT,(this->format==FORMAT["GL_RGBA"])?4:1);
            glGenTextures(1,&this->textureID);
            glBindTexture(GL_TEXTURE_2D,this->textureID);
            glTexImage2D(GL_TEXTURE_2D,0,this->format,this->width,this->height,0,this->format,this->type,0);
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,warp);
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,warp);
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,filter);
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,filter);
            if(isFbo) {
                glGenFramebuffers(1,&this->fbo);
                glBindFramebuffer(GL_FRAMEBUFFER,this->fbo);
                glGenRenderbuffers(1,&this->depthBuffer);
                glBindRenderbuffer(GL_RENDERBUFFER,this->depthBuffer);
                glRenderbufferStorage(GL_RENDERBUFFER,GL_DEPTH_COMPONENT16,this->width,this->height);
                glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,this->textureID,0);
                glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_RENDERBUFFER,this->depthBuffer);
                if(glCheckFramebufferStatus(GL_FRAMEBUFFER)!=GL_FRAMEBUFFER_COMPLETE) printf("Error : FBO\n");
                glBindFramebuffer(GL_FRAMEBUFFER,0);
                glBindRenderbuffer(GL_RENDERBUFFER,0);
            }
            else {
                this->fbo = 0; 
            }
        }
    
        ~Texture() {
            if(this->fbo) {
                glDeleteFramebuffers(1,&this->fbo);
                glDeleteRenderbuffers(1,&this->depthBuffer);
            }
            glDeleteTextures(1,&this->textureID);
        }
        
        GLuint get() {
            return this->textureID;
        }
    
        void bind() {            
            if(this->fbo) {
                glBindFramebuffer(GL_FRAMEBUFFER,this->fbo);
            }
        }
    
        void unbind() {
            glBindFramebuffer(GL_FRAMEBUFFER,0);
        }
    
        void begin(unsigned int t) {
            glActiveTexture(t);
            glBindTexture(GL_TEXTURE_2D,this->textureID);
        }

        void end() {
            glBindTexture(GL_TEXTURE_2D,0);
        }
    
        void replace(unsigned char *p, int x,int y, int w,int h) {
            if(!this->fbo&&p!=nullptr) {
                glBindTexture(GL_TEXTURE_2D,this->textureID);
                    glTexSubImage2D(GL_TEXTURE_2D,0,x,y,w,h,this->format,this->type,p);
                glBindTexture(GL_TEXTURE_2D,0);
            }
        }
    
        void replace(unsigned char *p, int w,int h) {
            this->replace(p,0,0,w,h);
        }
    
};