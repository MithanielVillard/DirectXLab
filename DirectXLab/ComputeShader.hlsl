StructuredBuffer<int> srcBuffer : register(t0);
RWStructuredBuffer<int> dstBuffer : register(u0);

[numthreads(32, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    dstBuffer[DTid.x] = srcBuffer[DTid.x] + 1;
}