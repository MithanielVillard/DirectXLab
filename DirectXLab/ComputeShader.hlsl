
cbuffer simulationTime : register(b2)
{
    float time;
}

RWStructuredBuffer<float4x4> transforms : register(u0);

[numthreads(32, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{

    float3 pos = float3(DTid.x, time, 0.0f);

    float4x4 world =
    {
        float4(1, 0, 0, 0),
        float4(0, 1, 0, 0),
        float4(0, 0, 1, 0),
        float4(pos, 1)
    };

    transforms[DTid.x] = world;
}