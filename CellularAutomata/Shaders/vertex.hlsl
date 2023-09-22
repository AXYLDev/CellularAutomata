struct Vertex {
	float3 pos : POSITION;
	float3 uv : UV;
};

struct VertexShaderOutput {
	float4 pos : SV_POSITION;
	float3 uv : UV;
};

VertexShaderOutput main(Vertex IN) {
	VertexShaderOutput OUT;
	OUT.pos = float4(IN.pos, 1);
	OUT.uv = IN.uv;
	return OUT;
}