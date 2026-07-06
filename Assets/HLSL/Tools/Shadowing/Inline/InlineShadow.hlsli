struct InlineShadowInput
{
	float3 position;
	float3 direction;
	float3 normal;
};

RaytracingAccelerationStructure gTLAS : register(t0, space3);

bool InlineShadow(InlineShadowInput input)
{
	RayDesc ray;
	ray.Direction = input.direction;
	ray.Origin = input.position + input.normal * 1e-4;
	ray.TMin = 0.0f;
	ray.TMax = 1000.0f;

	RayQuery<RAY_FLAG_SKIP_PROCEDURAL_PRIMITIVES |
						RAY_FLAG_ACCEPT_FIRST_HIT_AND_END_SEARCH> rq;
	RAY_FLAG flags = RAY_FLAG_SKIP_PROCEDURAL_PRIMITIVES |
						RAY_FLAG_ACCEPT_FIRST_HIT_AND_END_SEARCH;
	
	rq.TraceRayInline(gTLAS, flags, 0xFF, ray);
	
	rq.Proceed();
	
	return rq.CommittedStatus() == COMMITTED_TRIANGLE_HIT;
}
