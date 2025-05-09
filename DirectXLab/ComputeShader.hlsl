RWStructuredBuffer<float4x4> transforms : register(u0);

cbuffer simulationTime : register(b2)
{
    float time;
}

[numthreads(32, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{

    float3 pos = float3(DTid.x-5.0f, time-5.0f, 0.0f);

    float4x4 world =
    {
        float4(1, 0, 0, 0),
        float4(0, 1, 0, 0),
        float4(0, 0, 1, 0),
        float4(pos, 1)
    };

    transforms[DTid.x] = world;
}