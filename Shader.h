class Shader;

typedef void (*BLOCK)(W67*,Shader*);
typedef void (*glGetriv)(GLuint,GLenum,GLint*);
typedef void (*glGetInfoLog)(GLuint,GLsizei,GLsizei*,GLchar*);

class Shader {

    private:
    
        void createShader(const char *s,GLenum type) {            
            GLint result;
            GLuint shader = glCreateShader(type);
            glShaderSource(shader,1,&s,NULL);
            glCompileShader(shader);
            glGetShaderiv(shader,GL_COMPILE_STATUS,&result);
            if(result) {
                glAttachShader(this->program,shader);
            }
            else {
                this->getInfoLog(shader,glGetShaderiv,glGetShaderInfoLog);
            }
            glDeleteShader(shader);
        }
    
        void getInfoLog(GLuint target,glGetriv iv,glGetInfoLog infoLog) {
            GLint infoLen = 0;
            iv(target,GL_INFO_LOG_LENGTH,&infoLen);
                if(infoLen>=1) {
                    char *log = new char[infoLen];
                    infoLog(target,infoLen,NULL,log);
                    printf("%s\n",log);
                    delete[] log;
                }			
            }

    public:
    
        GLuint program;
    
        Shader(const char *vs,const char *fs) {
            this->program = glCreateProgram();			
            this->createShader(vs,GL_VERTEX_SHADER);
            this->createShader(fs,GL_FRAGMENT_SHADER);
            GLint result;
            glLinkProgram(this->program);
            glGetProgramiv(this->program,GL_LINK_STATUS,&result);
            if(!result) {
                this->getInfoLog(this->program,glGetProgramiv,glGetProgramInfoLog);
                this->program = 0;
            }
        }
    
        void setup() {}
        
        void begin() {        
            if(this->program) glUseProgram(this->program);
        }
    
        void end() {
            glUseProgram(0);
        }
    
        void perform(W67 *ctx,BLOCK callback) {
            if(this->program) {
                this->begin();
                    callback(ctx,this);
                this->end();
            }
        }
};

