StructuredBuffer<int> srcBuffer : register(t0);
RWStructuredBuffer<int> dstBuffer : register(u0);

[numthreads(16, 16, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{

}