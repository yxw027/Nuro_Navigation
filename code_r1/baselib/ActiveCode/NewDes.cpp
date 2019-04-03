#include "NewDes.h"

bool DES::CDesEnter(const unsigned char *in, unsigned char *out, int datalen, const unsigned char key[8], bool type) 
{ 
    if ((!in) || (!out) || (datalen<1) || (!key) || (type && ((datalen % 8) !=0))) 
    {
        return false; 
    }

    int i = 0;  
    
    if (type == 0)
    { 
        unsigned char te8bit[8] = {0, 0, 0, 0, 0, 0, 0, 0}; 
        int te_fixlen = datalen - (datalen % 8); 
        
        for( i = 0; i < (datalen % 8); i++) 
        {
            te8bit[i] = in[te_fixlen + i]; 
        }

        for(i = 0; i < te_fixlen; i += 8) 
        {
            endes(in + i, key, out + i); 
        }

        if(datalen % 8 != 0) 
        {
            endes(te8bit, key, out + datalen / 8 * 8); 
        }
    } 
    else 
    { 

        for(i = 0; i < datalen; i += 8) 
        {
            undes(in + i, key, out + i); 
        }
    } 

    return true; 
} 

bool DES::CDesMac(const unsigned char *mac_data, unsigned char *mac_code, int datalen, const unsigned char key[8]) 
{ 
    if ((!mac_data) || (!mac_code) || (datalen < 16) || (datalen % 8 != 0) || (!key)) 
    {
        return false; 
    }
    
    endes(mac_data, key, mac_code); 
    
    for (int i = 8; i < datalen; i += 8) 
    { 
        XOR(mac_code, mac_data + i, mac_code); 
        endes(mac_code, key, mac_code); 
    } 
    
    return true; 
} 

void DES::XOR(const unsigned char in1[8], const unsigned char in2[8], unsigned char out[8]) 
{ 
    for (int i = 0; i < 8; i++) 
    {
        out[i] = in1[i] ^ in2[i];  
    }
} 

unsigned char *DES::Bin2ASCII(const unsigned char byte[64], unsigned char bit[8]) 
{ 
    for (int i = 0; i < 8; i++) 
    { 
        bit[i] = byte[i * 8] * 128 + byte[i * 8 + 1] * 64 +  
            byte[i * 8 + 2] * 32 + byte[i * 8 + 3] * 16 +  
            byte[i * 8 + 4] * 8 + byte[i * 8 + 5] * 4 +  
            byte[i * 8 + 6] * 2 + byte[i * 8 + 7];  
    } 

    return bit; 
} 

unsigned char *DES::ASCII2Bin(const unsigned char bit[8], unsigned char byte[64]) 
{ 
    for (int i = 0; i < 8; i++) 
    {
        for (int j = 0; j < 8; j++) 
        {
            byte[i * 8 + j] = ( bit[i] >> (7 - j) ) & 0x01; 
        }
    }

    return byte; 
} 

void DES::GenSubKey(const unsigned char oldkey[8], unsigned char newkey[16][8]) 
{ 
    int i, k, rol = 0; 

    int pc_1[56] = {
        57, 49, 41, 33, 25, 17,  9, 
         1, 58, 50, 42, 34, 26, 18, 
        10,  2, 59, 51, 43, 35, 27, 
        19, 11,  3, 60, 52, 44, 36, 
        63, 55, 47, 39, 31, 23, 15, 
         7, 62, 54, 46, 38, 30, 22, 
        14,  6, 61, 53, 45, 37, 29, 
        21, 13, 5,  28, 20, 12,  4
    }; 

    int pc_2[48] = {
        14, 17, 11, 24,  1,  5, 
         3, 28, 15,  6, 21, 10, 
        23, 19, 12,  4, 26,  8, 
        16,  7, 27, 20, 13,  2, 
        41, 52, 31, 37, 47, 55, 
        30, 40, 51, 45, 33, 48, 
        44, 49, 39, 56, 34, 53, 
        46, 42, 50, 36, 29, 32}; 

    int ccmovebit[16] = {1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1}; 

    unsigned char oldkey_byte[64]; 
    unsigned char oldkey_byte1[64]; 
    unsigned char oldkey_byte2[64]; 
    unsigned char oldkey_c[56]; 
    unsigned char oldkey_d[56]; 
    unsigned char newkey_byte[16][64]; 

    ASCII2Bin(oldkey, oldkey_byte); 

    for(i = 0; i < 56; i++) 
    {
        oldkey_byte1[i] = oldkey_byte[pc_1[i] - 1]; 
    }

    for(i = 0; i < 28; i++) 
    {
        oldkey_c[i] = oldkey_byte1[i];
        oldkey_c[i + 28] = oldkey_byte1[i];
        oldkey_d[i] = oldkey_byte1[i + 28];
        oldkey_d[i + 28] = oldkey_byte1[i + 28];
    }

    for(i = 0; i < 16; i++) 
    {
        rol += ccmovebit[i]; 

        for(k = 0; k < 28; k++)
        {
            oldkey_byte2[k] = oldkey_c[k + rol], oldkey_byte2[k + 28] = oldkey_d[k + rol];
        }

        for(k = 0; k < 48; k++)
        {
            newkey_byte[i][k] = oldkey_byte2[pc_2[k] - 1];
        }

    } 

    for(i = 0; i < 16; i++)
    {
        Bin2ASCII(newkey_byte[i], newkey[i]);
    }
}

void DES::endes(const unsigned char m_bit[8], const unsigned char k_bit[8], unsigned char e_bit[8]) 
{ 
    int ip[64] = { 
        58, 50, 42, 34, 26, 18, 10, 2, 
        60, 52, 44, 36, 28, 20, 12, 4, 
        62, 54, 46, 38, 30, 22, 14, 6, 
        64, 56, 48, 40, 32, 24, 16, 8, 
        57, 49, 41, 33, 25, 17,  9, 1, 
        59, 51, 43, 35, 27, 19, 11, 3, 
        61, 53, 45, 37, 29, 21, 13, 5, 
        63, 55, 47, 39, 31, 23, 15, 7 
    }; 

    int ip_1[64] = { 
        40, 8, 48, 16, 56, 24, 64, 32,  
        39, 7, 47, 15, 55, 23, 63, 31,  
        38, 6, 46, 14, 54, 22, 62, 30,  
        37, 5, 45, 13, 53, 21, 61, 29,  
        36, 4, 44, 12, 52, 20, 60, 28,  
        35, 3, 43, 11, 51, 19, 59, 27,  
        34, 2, 42, 10, 50, 18, 58, 26,  
        33, 1, 41,  9, 49, 17, 57, 25 
    }; 

    int e[48] = { 
        32,  1,  2,  3,  4,  5,  
         4,  5,  6,  7,  8,  9,  
         8,  9, 10, 11, 12, 13,  
        12, 13, 14, 15, 16, 17,  
        16, 17, 18, 19, 20, 21,  
        20, 21, 22, 23, 24, 25,  
        24, 25, 26, 27, 28, 29,  
        28, 29, 30, 31, 32, 1 
    };

    unsigned char m_bit1[8] = {0}; 
    unsigned char m_byte[64] = {0}; 
    unsigned char m_byte1[64] = {0}; 
    unsigned char key_n[16][8] = {0}; 
    unsigned char l_bit[17][8] = {0}; 
    unsigned char r_bit[17][8] = {0}; 
    unsigned char e_byte[64] = {0}; 
    unsigned char e_byte1[64] = {0}; 
    unsigned char r_byte[64] = {0}; 
    unsigned char r_byte1[64] = {0}; 
    int i, j; 


    GenSubKey(k_bit, key_n); 

    ASCII2Bin(m_bit, m_byte); 

    for(i = 0; i < 64; i++) 
    {
        m_byte1[i] = m_byte[ip[i] - 1];
    }

    Bin2ASCII(m_byte1, m_bit1); 

    for(i = 0; i < 4; i++)
    {
        l_bit[0][i] = m_bit1[i], r_bit[0][i] = m_bit1[i + 4];
    }


    for (i = 1; i <= 16; i++) 
    { 
        for (j = 0; j < 4; j++)
        {
            l_bit[i][j] = r_bit[i-1][j];
        }

        ASCII2Bin(r_bit[i-1], r_byte); 

        for (j = 0; j < 48; j++)
        {
            r_byte1[j] = r_byte[e[j] - 1];
        }

        Bin2ASCII(r_byte1, r_bit[i-1]); 

        for (j = 0; j < 6; j++) 
        {
            r_bit[i-1][j] = r_bit[i-1][j] ^ key_n[i-1][j]; 
        }

        SReplace(r_bit[i - 1]); 

        for (j = 0; j < 4; j++) 
        { 
            r_bit[i][j] = l_bit[i-1][j] ^ r_bit[i-1][j]; 
        } 
    } 

    for (i = 0; i < 4; i++) 
    {
        e_bit[i] = r_bit[16][i], e_bit[i + 4] = l_bit[16][i]; 
    }

    ASCII2Bin(e_bit, e_byte); 

    for (i = 0; i < 64; i++) 
    {
        e_byte1[i] = e_byte[ip_1[i] - 1]; 
    }

    Bin2ASCII(e_byte1, e_bit); 
} 

void DES::undes(const unsigned char m_bit[8], const unsigned char k_bit[8], unsigned char e_bit[8]) 
{ 
    int ip[64] = { 
        58,50,42,34,26,18,10,2, 
        60,52,44,36,28,20,12,4, 
        62,54,46,38,30,22,14,6, 
        64,56,48,40,32,24,16,8, 
        57,49,41,33,25,17,9,1, 
        59,51,43,35,27,19,11,3, 
        61,53,45,37,29,21,13,5, 
        63,55,47,39,31,23,15,7 
    }; 

    int ip_1[64] = { 
        40,8,48,16,56,24,64,32, 
        39,7,47,15,55,23,63,31, 
        38,6,46,14,54,22,62,30, 
        37,5,45,13,53,21,61,29, 
        36,4,44,12,52,20,60,28, 
        35,3,43,11,51,19,59,27, 
        34,2,42,10,50,18,58,26, 
        33,1,41,9,49,17,57,25 
    }; 

    int e[48] = { 
        32,1, 2, 3, 4, 5, 
        4, 5, 6, 7, 8, 9, 
        8, 9, 10,11,12,13, 
        12,13,14,15,16,17, 
        16,17,18,19,20,21, 
        20,21,22,23,24,25, 
        24,25,26,27,28,29, 
        28,29,30,31,32,1 
    }; 
    
    unsigned char m_bit1[8] = {0}; 
    unsigned char m_byte[64] = {0}; 
    unsigned char m_byte1[64] = {0}; 
    unsigned char key_n[16][8] = {0}; 
    unsigned char l_bit[17][8] = {0}; 
    unsigned char r_bit[17][8] = {0}; 
    unsigned char e_byte[64] = {0}; 
    unsigned char e_byte1[64] = {0}; 
    unsigned char l_byte[64] = {0}; 
    unsigned char l_byte1[64] = {0}; 
    int i = 0, j = 0; 


    GenSubKey(k_bit, key_n); 

    ASCII2Bin(m_bit, m_byte); 

    for (i = 0; i < 64; i++) 
    {
        m_byte1[i] = m_byte[ip[i] - 1]; 
    }

    Bin2ASCII(m_byte1, m_bit1); 

    for (i = 0; i < 4; i++) 
    {
        r_bit[16][i] = m_bit1[i], l_bit[16][i] = m_bit1[i + 4]; 
    }


    for (i = 16; i > 0; i--) 
    { 

        for (j = 0; j < 4; j++) 
        {
            r_bit[i-1][j] = l_bit[i][j]; 
        }

        ASCII2Bin(l_bit[i], l_byte); 

        for (j = 0; j < 48; j++) 
        {
            l_byte1[j] = l_byte[e[j] - 1]; 
        }

        Bin2ASCII(l_byte1, l_bit[i]); 


        for (j = 0; j < 6; j++) 
        {
            l_bit[i][j] = l_bit[i][j] ^ key_n[i-1][j]; 
        }

        SReplace(l_bit[i]); 

        for (j = 0; j < 4; j++) 
        { 
            l_bit[i-1][j] = r_bit[i][j] ^ l_bit[i][j]; 
        } 
    } 

    for (i = 0; i < 4; i++) 
    {
        e_bit[i] = l_bit[0][i], e_bit[i + 4] = r_bit[0][i]; 
    }

    ASCII2Bin(e_bit, e_byte); 

    for (i = 0; i < 64; i++) 
    {
        e_byte1[i] = e_byte[ip_1[i] - 1]; 
    }

    Bin2ASCII(e_byte1, e_bit); 
} 

void DES::SReplace(unsigned char s_bit[8]) 
{ 
    int p[32] = { 
        16,7,20,21, 
        29,12,28,17, 
        1,15,23,26, 
        5,18,31,10, 
        2,8,24,14, 
        32,27,3,9, 
        19,13,30,6, 
        22,11,4,25 
    }; 
    unsigned char s[][4][16] ={  
        { 
            14,4,13,1,2,15,11,8,3,10,6,12,5,9,0,7, 
            0,15,7,4,14,2,13,1,10,6,12,11,9,5,3,8, 
            4,1,14,8,13,6,2,11,15,12,9,7,3,10,5,0, 
            15,12,8,2,4,9,1,7,5,11,3,14,10,0,6,13 
        }, 
        { 
            15,1,8,14,6,11,3,4,9,7,2,13,12,0,5,10, 
            3,13,4,7,15,2,8,14,12,0,1,10,6,9,11,5, 
            0,14,7,11,10,4,13,1,5,8,12,6,9,3,2,15, 
            13,8,10,1,3,15,4,2,11,6,7,12,0,5,14,9 
        }, 
        { 
            10,0,9,14,6,3,15,5,1,13,12,7,11,4,2,8, 
            13,7,0,9,3,4,6,10,2,8,5,14,12,11,15,1, 
            13,6,4,9,8,15,3,0,11,1,2,12,5,10,14,7, 
            1,10,13,0,6,9,8,7,4,15,14,3,11,5,2,12 
        }, 
        { 
            7,13,14,3,0,6,9,10,1,2,8,5,11,12,4,15, 
            13,8,11,5,6,15,0,3,4,7,2,12,1,10,14,9, 
            10,6,9,0,12,11,7,13,15,1,3,14,5,2,8,4, 
            3,15,0,6,10,1,13,8,9,4,5,11,12,7,2,14 
        }, 
        { 
            2,12,4,1,7,10,11,6,8,5,3,15,13,0,14,9, 
            14,11,2,12,4,7,13,1,5,0,15,10,3,9,8,6, 
            4,2,1,11,10,13,7,8,15,9,12,5,6,3,0,14, 
            11,8,12,7,1,14,2,13,6,15,0,9,10,4,5,3, 
        }, 
        { 
            12,1,10,15,9,2,6,8,0,13,3,4,14,7,5,11, 
            10,15,4,2,7,12,9,5,6,1,13,14,0,11,3,8, 
            9,14,15,5,2,8,12,3,7,0,4,10,1,13,11,6, 
            4,3,2,12,9,5,15,10,11,14,1,7,6,0,8,13 
        }, 
        { 
            4,11,2,14,15,0,8,13,3,12,9,7,5,10,6,1, 
            13,0,11,7,4,9,1,10,14,3,5,12,2,15,8,6, 
            1,4,11,13,12,3,7,14,10,15,6,8,0,5,9,2, 
            6,11,13,8,1,4,10,7,9,5,0,15,14,2,3,12 
        }, 
        { 
            13,2,8,4,6,15,11,1,10,9,3,14,5,0,12,7, 
            1,15,13,8,10,3,7,4,12,5,6,11,0,14,9,2, 
            7,11,4,1,9,12,14,2,0,6,10,13,15,3,5,8, 
            2,1,14,7,4,10,8,13,15,12,9,0,3,5,6,11 
        } 
    }; 
    
    unsigned char s_byte[64] = {0}; 
    unsigned char s_byte1[64] = {0}; 
    unsigned char row = 0, col = 0; 
    unsigned char s_out_bit[8] = {0}; 

    ASCII2Bin(s_bit, s_byte); 

    int i = 0;
    
    for(i = 0; i < 8; i++) 
    { 

        row = s_byte[i * 6] * 2 + s_byte[i * 6 + 5]; 
        col = s_byte[i * 6 + 1] * 8 + s_byte[i * 6 + 2] * 4 + s_byte[i * 6 + 3] * 2 + s_byte[i * 6 + 4]; 
        s_out_bit[i] = s[i][row][col]; 
    } 

    s_out_bit[0] = (s_out_bit[0] << 4) + s_out_bit[1]; 
    s_out_bit[1] = (s_out_bit[2] << 4) + s_out_bit[3]; 
    s_out_bit[2] = (s_out_bit[4] << 4) + s_out_bit[5]; 
    s_out_bit[3] = (s_out_bit[6] << 4) + s_out_bit[7]; 

    ASCII2Bin(s_out_bit, s_byte); 

    for(i = 0; i < 32; i++) 
        s_byte1[i] = s_byte[p[i] - 1]; 

    Bin2ASCII(s_byte1, s_bit); 
} 

