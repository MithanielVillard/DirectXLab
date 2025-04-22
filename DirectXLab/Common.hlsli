struct VertexIn
{
    float3 posL : POSITION;
    float4 color : COLOR;
    float2 uv : UV;
};

struct VertexOut
{
    float4 posH : SV_POSITION;
    float4 color : COLOR;
    float2 uv : UV;
};