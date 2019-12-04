class ColorMatrix {

	private:
		
		float _mat[20];
		float _raw[20];
		float _matrix[16];
		float _offset[4];
				
		void concat(float *a,float *b) {
			static float tmp[20];
			for(int i=0; i<4; i++) {			
				tmp[i*5+0]=a[i*5]*b[0]+a[i*5+1]*b[5+0]+a[i*5+2]*b[10+0]+a[i*5+3]*b[15+0];
				tmp[i*5+1]=a[i*5]*b[1]+a[i*5+1]*b[5+1]+a[i*5+2]*b[10+1]+a[i*5+3]*b[15+1];
				tmp[i*5+2]=a[i*5]*b[2]+a[i*5+1]*b[5+2]+a[i*5+2]*b[10+2]+a[i*5+3]*b[15+2];
				tmp[i*5+3]=a[i*5]*b[3]+a[i*5+1]*b[5+3]+a[i*5+2]*b[10+3]+a[i*5+3]*b[15+3];
				tmp[i*5+4]=a[i*5]*b[4]+a[i*5+1]*b[5+4]+a[i*5+2]*b[10+4]+a[i*5+3]*b[15+4];
			}
			for(int i=0; i<20; i++) b[i]=tmp[i];
		}

		void identity(float *m) {
			for(int i=0; i<20; i++) m[i]=0.0;
			m[ 0]=1.0;
			m[ 6]=1.0;
			m[12]=1.0;
			m[18]=1.0;
		}
		
	public:
		
		ColorMatrix() {
			this->identity();
		}
		
		~ColorMatrix() {
		}

		ColorMatrix *identity() {
			this->identity(this->_raw);
			return this;
		}

		ColorMatrix *saturation(float sat){
			this->identity(this->_mat);
			float s = 1.0-sat;
			float r = s*0.212671;
			float g = s*0.715160;
			float b = s*0.072169;
			this->_mat[ 0] = r+sat; 
			this->_mat[ 1] = g; 
			this->_mat[ 2] = b;
			this->_mat[ 5] = r; 
			this->_mat[ 6] = g+sat; 
			this->_mat[ 7] = b;
			this->_mat[10] = r; 
			this->_mat[11] = g; 
			this->_mat[12] = b+sat;
			this->_mat[18] = 1.0;	
			concat(this->_mat,this->_raw);
			return this;
		}

		ColorMatrix *contrast(float r,float g,float b) {
			this->identity(this->_mat);
			this->_mat[ 0] = r+1.0; 
			this->_mat[ 4] = -(128.0*r);
			this->_mat[ 6] = g+1.0;  
			this->_mat[ 9] = -(128.0*g);
			this->_mat[12] = b+1.0;
			this->_mat[14] = -(128.0*b);
			this->_mat[18] = 1.;			
			concat(this->_mat,this->_raw);			
			return this;
		}
	
		ColorMatrix *contrast(float v) {
			return contrast(v,v,v);
		}

		float *matrix() {
			for(int i=0; i<4; i++) {
				this->_matrix[(i<<2)+0] = this->_raw[(i<<2)+0+i];
				this->_matrix[(i<<2)+1] = this->_raw[(i<<2)+1+i];
				this->_matrix[(i<<2)+2] = this->_raw[(i<<2)+2+i];
				this->_matrix[(i<<2)+3] = this->_raw[(i<<2)+3+i];
			}
			return this->_matrix;
		}

		float *offset() {
			this->_offset[0] = this->_raw[ 4]*(1.0/255.0);
			this->_offset[1] = this->_raw[ 9]*(1.0/255.0);
			this->_offset[2] = this->_raw[14]*(1.0/255.0);
			this->_offset[3] = this->_raw[19]*(1.0/255.0);
			return this->_offset;
		}
};