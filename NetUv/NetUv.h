#include "uv.h"

namespace NetUv
{
    public ref class UvTcp sealed
    {
    public:
        ~UvTcp();
    internal:
        UvTcp(uv_loop_t* loop);
    private:
        uv_tcp_t* _tcp;
    };

    public ref class UvLoop sealed
    {
    public:
        UvLoop();
        ~UvLoop();
        void Run();
    private:
        uv_loop_t* _loop;
    };
}