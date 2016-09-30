#pragma once

struct stMaterial
{
	stMaterial()
	{
		emissiveR = emissiveG = emissiveB = emissiveA = 0;
		ambientR = ambientG = ambientB = ambientA = 1;
		diffuseR = diffuseG = diffuseB = diffuseA = 1;
		specularR = specularG = specularB = specularA = 0;
		power = 0;
	}

	float emissiveR, emissiveG, emissiveB, emissiveA;
	float ambientR, ambientG, ambientB, ambientA;
	float diffuseR, diffuseG, diffuseB, diffuseA;
	float specularR, specularG, specularB, specularA;
	float power;
};