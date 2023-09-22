struct FragInput {
	float2 uv : UV;
};
struct FragOutput {
	float4 color : COLOR0;
};

FragOutput main(FragInput IN) : SV_Target {
	FragOutput output;
	output.color = float4(1, IN.uv.xy, 1);
	return output;
}