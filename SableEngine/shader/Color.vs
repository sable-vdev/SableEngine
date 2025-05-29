//Globals
cbuffer Matrixbuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
}

//Typedefs

struct VertexInput 
{
	float4 position : POSITION;
	float4 color : COLOR;
};

struct PixelInput
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

//Vertex Shader

PixelInput main(VertexInput input) 
{
	PixelInput output;

	//Change the position vector to be 4 units for proper matrix calculations
	input.position.w = 1.0f;

	//Calculate the position of the vertex against the matrices
	output.position = mul(input.position, worldMatrix);
	output.position = mul(input.position, viewMatrix);
	output.position = mul(input.position, projectionMatrix);

	//Store the input color for the pixel shader to use
	output.color = input.color;

	return output;
}