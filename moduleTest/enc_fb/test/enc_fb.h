#ifndef ENC_FB_H_
#define ENC_FB_H

struct enc_screen_info{
    int width;
    int height;
    int bits_per_pixel;
};
struct enc_info{
    int frame_rate;
    int profile; //baseline(66) main(77) high(100)
    unsigned char enc_status; //0-destory 1-start 2-stop
    unsigned char set_i_frame;//消费者设置i帧，生产者读取到该数据后会将下一帧设置为i帧，并将set_i_frame设置为0，恢复到自动设置i帧方式
    unsigned char need_read_frame;//当设置为1时，消费者才可以读取帧数据，并在读取后设置为0，这样保证消费者不会读取重复帧也不会丢帧。帧同步由生产者处理
};
struct enc_data{
    int size;
    unsigned char* data;
};

#define ENC_FB_IOC_MAGIC '$'
#define ENC_FB_IOC_MAXNR 8
//获取屏幕信息
#define ENC_FB_IOGET_SCREEN_INFO _IOR(ENC_FB_IOC_MAGIC, 0, struct enc_info *)
//设置屏幕信息
#define ENC_FB_IOSET_SCREEN_INFO _IOW(ENC_FB_IOC_MAGIC, 1, struct enc_info *)
//获取编码信息
#define ENC_FB_IOGET_ENC_INFO _IOR(ENC_FB_IOC_MAGIC, 2, struct enc_info *)
//设置编码信息
#define ENC_FB_IOSET_ENC_INFO _IOW(ENC_FB_IOC_MAGIC, 3, struct enc_info *)
//获取当前帧的数据内容
#define ENC_FB_IOGET_DATA _IOR(ENC_FB_IOC_MAGIC, 4, unsigned char *)
//设置当前帧的数据内容
#define ENC_FB_IOSET_DATA _IOW(ENC_FB_IOC_MAGIC, 5, unsigned char *)
//设置数据长度
#define ENC_FB_IOSET_SIZE _IO(ENC_FB_IOC_MAGIC, 6)//通过引用参数值设置
//设置帧同步
#define ENC_FB_IOSET_VSYNC _IO(ENC_FB_IOC_MAGIC, 7)//通过引用参数值设置
//读取帧同步
#define ENC_FB_IOGET_VSYNC _IO(ENC_FB_IOC_MAGIC, 8) //返回值为帧同步


#endif
