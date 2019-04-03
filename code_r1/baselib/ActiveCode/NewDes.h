#ifndef _DES_ENCRYPT_DECRYPT 
#define _DES_ENCRYPT_DECRYPT

class DES 
{ 
    public: 
        bool CDesEnter(const unsigned char *in, unsigned char *out, int datalen, const unsigned char key[8], bool type);
        bool CDesMac(const unsigned char *mac_data, unsigned char *mac_code, int datalen, const unsigned char key[8]); 

    private: 
        void XOR(const unsigned char in1[8], const unsigned char in2[8], unsigned char out[8]); 
        unsigned char *Bin2ASCII(const unsigned char byte[64], unsigned char bit[8]); 
        unsigned char *ASCII2Bin(const unsigned char bit[8], unsigned char byte[64]); 
        void GenSubKey(const unsigned char oldkey[8], unsigned char newkey[16][8]); 
        void endes(const unsigned char m_bit[8], const unsigned char k_bit[8], unsigned char e_bit[8]); 
        void undes(const unsigned char m_bit[8], const unsigned char k_bit[8], unsigned char e_bit[8]); 
        void SReplace(unsigned char s_bit[8]); 
}; 

#endif
