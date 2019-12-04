class Plane {
    
    private:
    
        GLuint buffers[2];
    
    public:
    
        Plane() {			
            glGenBuffers(2,buffers);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,buffers[0]);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(short *)*6,new short[6]{
                    0, 1, 2,
                    3, 2, 1
                },GL_STATIC_DRAW);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
            glBindBuffer(GL_ARRAY_BUFFER,buffers[1]);
                glBufferData(GL_ARRAY_BUFFER,sizeof(float *)*12,new float[12]{
                    -1.0, 1.0, 0.0,
                     1.0, 1.0, 0.0,
                    -1.0,-1.0, 0.0,
                     1.0,-1.0, 0.0
                },GL_STATIC_DRAW);
                
            glBindBuffer(GL_ARRAY_BUFFER,0);
        }
    
        void begin() {
            glBindBuffer(GL_ARRAY_BUFFER,buffers[1]);
            glEnableVertexAttribArray(0);
        }
    
        void end() {
            glDisableVertexAttribArray(0);
            glBindBuffer(GL_ARRAY_BUFFER,0);
        }
    
        void draw() {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,buffers[0]);
                glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_SHORT,0);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
        }
    
};