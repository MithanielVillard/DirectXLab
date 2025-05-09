struct VertexIn
{
    float3 posL : POSITION;
    float4 color : COLOR;
    float2 uv : UV;
	uint instanceID : SV_InstanceID;
};

struct VertexOut
{
    float4 posH : SV_POSITION;
    float4 color : COLOR;
    float2 uv : UV;
    uint instanceID : SV_InstanceID;
};