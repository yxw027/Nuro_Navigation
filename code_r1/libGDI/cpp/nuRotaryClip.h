#ifndef _NUROTARYCLIP_H_JKLFA89234JLFAN
#define _NUROTARYCLIP_H_JKLFA89234JLFAN



class TRotaryClipData
{
public:
    long src_width;
    long src_height;
    long dst_width;
    long dst_height;
    long Ax_16; 
    long Ay_16; 
    long Bx_16; 
    long By_16; 
    long Cx_16;
    long Cy_16; 
    long border_width;//插值边界宽度
private:
    long cur_dst_up_x0;
    long cur_dst_up_x1;
    long cur_dst_down_x0;
    long cur_dst_down_x1;
    inline bool is_border_src(long src_x_16,long src_y_16)
    {
         return ( ( (src_x_16>=(-(border_width<<16)))&&((src_x_16>>16)<(src_width +border_width)) )
               && ( (src_y_16>=(-(border_width<<16)))&&((src_y_16>>16)<(src_height+border_width)) ) );
    }
    inline bool is_in_src(long src_x_16,long src_y_16)
    {
         return ( ( (src_x_16>=(border_width<<16))&&((src_x_16>>16)<(src_width-border_width) ) )
               && ( (src_y_16>=(border_width<<16))&&((src_y_16>>16)<(src_height-border_width)) ) );
    }
    void find_begin_in(long dst_y,long& out_dst_x,long& src_x_16,long& src_y_16)
    {
        src_x_16-=Ax_16;
        src_y_16-=Ay_16;
        while (is_border_src(src_x_16,src_y_16))
        {
            --out_dst_x;
            src_x_16-=Ax_16;
            src_y_16-=Ay_16;
        }
        src_x_16+=Ax_16;
        src_y_16+=Ay_16;
    }
    bool find_begin(long dst_y,long& out_dst_x0,long dst_x1)
    {
        long test_dst_x0=out_dst_x0-1;
        long src_x_16=Ax_16*test_dst_x0 + Bx_16*dst_y + Cx_16;
        long src_y_16=Ay_16*test_dst_x0 + By_16*dst_y + Cy_16;
        for (long i=test_dst_x0;i<=dst_x1;++i)
        {
            if (is_border_src(src_x_16,src_y_16))
            {
                out_dst_x0=i;
                if (i==test_dst_x0)
                    find_begin_in(dst_y,out_dst_x0,src_x_16,src_y_16);
                if (out_dst_x0<0)
                {
                    src_x_16-=(Ax_16*out_dst_x0);
                    src_y_16-=(Ay_16*out_dst_x0);
                }
                out_src_x0_16=src_x_16;
                out_src_y0_16=src_y_16;
                return true;
            }
            else
            {
                src_x_16+=Ax_16;
                src_y_16+=Ay_16;
            }
        }
        return false;
    }
    void find_end(long dst_y,long dst_x0,long& out_dst_x1)
    {
        long test_dst_x1=out_dst_x1;
        if (test_dst_x1<dst_x0) test_dst_x1=dst_x0;

        long src_x_16=Ax_16*test_dst_x1 + Bx_16*dst_y + Cx_16;
        long src_y_16=Ay_16*test_dst_x1 + By_16*dst_y + Cy_16;
        if (is_border_src(src_x_16,src_y_16))
        {
            ++test_dst_x1;
            src_x_16+=Ax_16;
            src_y_16+=Ay_16;
            while (is_border_src(src_x_16,src_y_16))
            {
                ++test_dst_x1;
                src_x_16+=Ax_16;
                src_y_16+=Ay_16;
            }
            out_dst_x1=test_dst_x1;
        }
        else
        {
            src_x_16-=Ax_16;
            src_y_16-=Ay_16;
            while (!is_border_src(src_x_16,src_y_16))
            {
                --test_dst_x1;
                src_x_16-=Ax_16;
                src_y_16-=Ay_16;
            }
            out_dst_x1=test_dst_x1;
        }
    }

    inline void update_out_dst_x_in()
    {
        if ((0==border_width)||(out_dst_x0_boder>=out_dst_x1_boder) )
        {
            out_dst_x0_in=out_dst_x0_boder;
            out_dst_x1_in=out_dst_x1_boder;
        }
        else
        {
            long src_x_16=out_src_x0_16;
            long src_y_16=out_src_y0_16;
            long i=out_dst_x0_boder;
            while (i<out_dst_x1_boder)
            {
                if (is_in_src(src_x_16,src_y_16)) break;
                src_x_16+=Ax_16;
                src_y_16+=Ay_16;
                ++i;
            }
            out_dst_x0_in=i;

            src_x_16=out_src_x0_16+(out_dst_x1_boder-out_dst_x0_boder)*Ax_16;
            src_y_16=out_src_y0_16+(out_dst_x1_boder-out_dst_x0_boder)*Ay_16;
            i=out_dst_x1_boder;
            while (i>out_dst_x0_in)
            {
                src_x_16-=Ax_16;
                src_y_16-=Ay_16;
                if (is_in_src(src_x_16,src_y_16)) break;
                --i;
            }
            out_dst_x1_in=i;
        }
    }
    inline void update_out_dst_up_x()
    {
        if (cur_dst_up_x0<0)
            out_dst_x0_boder=0;
        else
            out_dst_x0_boder=cur_dst_up_x0;
        if (cur_dst_up_x1>=dst_width)
            out_dst_x1_boder=dst_width;
        else
            out_dst_x1_boder=cur_dst_up_x1;
        update_out_dst_x_in();
    }
    inline void update_out_dst_down_x()
    {
        if (cur_dst_down_x0<0)
            out_dst_x0_boder=0;
        else
            out_dst_x0_boder=cur_dst_down_x0;
        if (cur_dst_down_x1>=dst_width)
            out_dst_x1_boder=dst_width;
        else
            out_dst_x1_boder=cur_dst_down_x1;
        update_out_dst_x_in();
    }

public:
    long out_src_x0_16;
    long out_src_y0_16;

    long out_dst_up_y;
    long out_dst_down_y;

    long out_dst_x0_boder;
    long out_dst_x0_in;
    long out_dst_x1_in;
    long out_dst_x1_boder;

public:
    bool inti_clip(long centerx, long centery,unsigned long aborder_width)
    {
        border_width=aborder_width;

        //计算src中心点映射到dst后的坐标
        out_dst_down_y=centery;//
        cur_dst_down_x0=centerx;//
        cur_dst_down_x1=cur_dst_down_x0;
        //得到初始扫描线
        if (find_begin(out_dst_down_y,cur_dst_down_x0,cur_dst_down_x1))
            find_end(out_dst_down_y,cur_dst_down_x0,cur_dst_down_x1);
        out_dst_up_y=out_dst_down_y;
        cur_dst_up_x0=cur_dst_down_x0;
        cur_dst_up_x1=cur_dst_down_x1;
        update_out_dst_up_x();
        return (cur_dst_down_x0<cur_dst_down_x1);
    }
    bool next_clip_line_down()
    {
        ++out_dst_down_y;
        if (!find_begin(out_dst_down_y,cur_dst_down_x0,cur_dst_down_x1)) return false;
        find_end(out_dst_down_y,cur_dst_down_x0,cur_dst_down_x1);
        update_out_dst_down_x();
        return (cur_dst_down_x0<cur_dst_down_x1);
    }
    bool next_clip_line_up()
    {
        --out_dst_up_y;
        if (!find_begin(out_dst_up_y,cur_dst_up_x0,cur_dst_up_x1)) return false;
        find_end(out_dst_up_y,cur_dst_up_x0,cur_dst_up_x1);
        update_out_dst_up_x();
        return (cur_dst_up_x0<cur_dst_up_x1);
    }
};

#endif //_NUROTARYCLIP_H_JKLFA89234JLFAN