#pragma once

Model* GenerateTerrain(TextureData *tex)
{
	int vertexCount = tex->width * tex->height;
	int triangleCount = (tex->width-1) * (tex->height-1) * 2;
	unsigned int x, z;
	
	vec3 *vertexArray = (vec3 *)malloc(sizeof(GLfloat) * 3 * vertexCount);
	vec3 *normalArray = (vec3 *)malloc(sizeof(GLfloat) * 3 * vertexCount);
	vec2 *texCoordArray = (vec2 *)malloc(sizeof(GLfloat) * 2 * vertexCount);
	GLuint *indexArray = (GLuint *) malloc(sizeof(GLuint) * triangleCount*3);
	
	//printf("bpp %d\n", tex->bpp);
	for (x = 0; x < tex->width; x++)
		for (z = 0; z < tex->height; z++)
		{
// Vertex array. You need to scale this properly
			vertexArray[(x + z * tex->width)].x = x / 1.0;
			vertexArray[(x + z * tex->width)].y = tex->imageData[(x + z * tex->width) * (tex->bpp/8)] / 5.0;
			vertexArray[(x + z * tex->width)].z = z / 1.0;
			//printf("%d", tex->width);
// Texture coordinates. You may want to scale them.
			texCoordArray[(x + z * tex->width)].x = (float)z/(tex->height-1)*5; // (float)x / tex->width;
			texCoordArray[(x + z * tex->width)].y = (float)x/(tex->width-1)*5; // (float)z / tex->height;
		}
// Make sure the vertices tile at edges

	for (x = 0; x < tex->width; x++)
	{
		vertexArray[(x+(tex->height-1)*(tex->width))].y = vertexArray[(x)].y;
	}
	for (z = 0; z < tex->height; z++)
	{
		vertexArray[((tex->width-1)+z*(tex->width))].y = vertexArray[(z*(tex->width))].y;
	}

	for (x = 0; x < tex->width-1; x++)
		for (z = 0; z < tex->height-1; z++)
		{
		// Triangle 1
			indexArray[(x + z * (tex->width-1))*6 + 0] = x + z * tex->width;
			indexArray[(x + z * (tex->width-1))*6 + 1] = x + (z+1) * tex->width;
			indexArray[(x + z * (tex->width-1))*6 + 2] = x+1 + z * tex->width;
		// Triangle 2
			indexArray[(x + z * (tex->width-1))*6 + 3] = x+1 + z * tex->width;
			indexArray[(x + z * (tex->width-1))*6 + 4] = x + (z+1) * tex->width;
			indexArray[(x + z * (tex->width-1))*6 + 5] = x+1 + (z+1) * tex->width;
		}
// Calc normals
	for (x = 0; x < tex->width; x++)
		for (z = 0; z < tex->height; z++)
		{
			vec3 normal = vec3{0,0,0};
			vec3 currentPos = vertexArray[(x + z*tex->width)];
			if( (x!=0) && (z!=0) && (z!=tex->width-1) && (x!=tex->height-1))
			{
				normal += normalize(cross(vertexArray[(x) + (z-1)*tex->width]-currentPos, vertexArray[(x-1) + (z)*tex->width]-currentPos));
				normal += 0.5*normalize(cross(vertexArray[(x+1) + (z-1)*tex->width]-currentPos, vertexArray[(x) + (z-1)*tex->width]-currentPos));
				normal += 0.5*normalize(cross(vertexArray[(x+1) + (z)*tex->width]-currentPos, vertexArray[(x+1) + (z-1)*tex->width]-currentPos));
				normal += normalize(cross(vertexArray[(x) + (z+1)*tex->width]-currentPos, vertexArray[(x+1) + (z)*tex->width]-currentPos));
				normal += 0.5*normalize(cross(vertexArray[(x-1) + (z+1)*tex->width]-currentPos, vertexArray[(x) + (z+1)*tex->width]-currentPos));
				normal += 0.5*normalize(cross(vertexArray[(x-1) + (z)*tex->width]-currentPos, vertexArray[(x-1) + (z+1)*tex->width]-currentPos));
				normalArray[(x + z * tex->width)] = normalize(normal);
			} else
			if( (z!=0) && (z!=tex->width-1) && (x==tex->height-1))
			{
				vec3 currentTemp = vertexArray[(0 + z*tex->width)];
				normal += 0.5*normalize(cross(vertexArray[(1) + (z-1)*tex->width]-currentTemp, vertexArray[(0) + (z-1)*tex->width]-currentTemp));
				normal += 0.5*normalize(cross(vertexArray[(1) + (z)*tex->width]-currentTemp, vertexArray[(1) + (z-1)*tex->width]-currentTemp));
				normal += normalize(cross(vertexArray[(0) + (z+1)*tex->width]-currentTemp, vertexArray[(1) + (z)*tex->width]-currentTemp));

				normal += normalize(cross(vertexArray[(x) + (z-1)*tex->width]-currentPos, vertexArray[(x-1) + (z)*tex->width]-currentPos));
				normal += 0.5*normalize(cross(vertexArray[(x-1) + (z+1)*tex->width]-currentPos, vertexArray[(x) + (z+1)*tex->width]-currentPos));
				normal += 0.5*normalize(cross(vertexArray[(x-1) + (z)*tex->width]-currentPos, vertexArray[(x-1) + (z+1)*tex->width]-currentPos));
				normalArray[(x + z * tex->width)] = normalize(normal);
			} else
			if( (x==0) && (z!=0) && (z!=tex->width-1) )
			{
				vec3 currentTemp = vertexArray[(tex->width-1 + z*tex->width)];
				normal += 0.5*normalize(cross(vertexArray[(x+1) + (z-1)*tex->width]-currentPos, vertexArray[(x) + (z-1)*tex->width]-currentPos));
				normal += 0.5*normalize(cross(vertexArray[(x+1) + (z)*tex->width]-currentPos, vertexArray[(x+1) + (z-1)*tex->width]-currentPos));
				normal += normalize(cross(vertexArray[(x) + (z+1)*tex->width]-currentPos, vertexArray[(x+1) + (z)*tex->width]-currentPos));

				normal += normalize(cross(vertexArray[(tex->width-1) + (z-1)*tex->width]-currentTemp, vertexArray[(tex->width-1-1) + (z)*tex->width]-currentTemp));
				normal += 0.5*normalize(cross(vertexArray[(tex->width-1-1) + (z+1)*tex->width]-currentTemp, vertexArray[(tex->width-1) + (z+1)*tex->width]-currentTemp));
				normal += 0.5*normalize(cross(vertexArray[(tex->width-1-1) + (z)*tex->width]-currentTemp, vertexArray[(tex->width-1-1) + (z+1)*tex->width]-currentTemp));
				normalArray[(x + z * tex->width)] = normalize(normal);
			} else
			if( (x!=0) && (x!=tex->height-1) && (z==tex->width-1) )
			{
				vec3 currentTemp = vertexArray[(x + (0)*tex->width)];
				normal += normalize(cross(vertexArray[(x) + (z-1)*tex->width]-currentPos, vertexArray[(x-1) + (z)*tex->width]-currentPos));
				normal += 0.5*normalize(cross(vertexArray[(x+1) + (z-1)*tex->width]-currentPos, vertexArray[(x) + (z-1)*tex->width]-currentPos));
				normal += 0.5*normalize(cross(vertexArray[(x+1) + (z)*tex->width]-currentPos, vertexArray[(x+1) + (z-1)*tex->width]-currentPos));
				
				normal += normalize(cross(vertexArray[(x) + (1)*tex->width]-currentTemp, vertexArray[(x+1) + (0)*tex->width]-currentTemp));
				normal += 0.5*normalize(cross(vertexArray[(x-1) + (1)*tex->width]-currentTemp, vertexArray[(x) + (1)*tex->width]-currentTemp));
				normal += 0.5*normalize(cross(vertexArray[(x-1) + (0)*tex->width]-currentTemp, vertexArray[(x-1) + (1)*tex->width]-currentTemp));
				normalArray[(x + z * tex->width)] = normalize(normal);
			} else
			if( (x!=0) && (x!=tex->height-1) && (z==0) )
			{
				vec3 currentTemp = vertexArray[(x + (tex->height-1)*tex->width)];
				normal += normalize(cross(vertexArray[(x) + (tex->height-2)*tex->width]-currentTemp, vertexArray[(x-1) + (tex->height-1)*tex->width]-currentTemp));
				normal += 0.5*normalize(cross(vertexArray[(x+1) + (tex->height-2)*tex->width]-currentTemp, vertexArray[(x) + (tex->height-2)*tex->width]-currentTemp));
				normal += 0.5*normalize(cross(vertexArray[(x+1) + (tex->height-1)*tex->width]-currentTemp, vertexArray[(x+1) + (tex->height-2)*tex->width]-currentTemp));
				
				normal += normalize(cross(vertexArray[(x) + (1)*tex->width]-currentPos, vertexArray[(x+1) + (0)*tex->width]-currentPos));
				normal += 0.5*normalize(cross(vertexArray[(x-1) + (1)*tex->width]-currentPos, vertexArray[(x) + (1)*tex->width]-currentPos));
				normal += 0.5*normalize(cross(vertexArray[(x-1) + (0)*tex->width]-currentPos, vertexArray[(x-1) + (1)*tex->width]-currentPos));
				normalArray[(x + z * tex->width)] = normalize(normal);
			} else
			{
				normalArray[(x + z * tex->width)] = vec3{0,1,0};
			}
		}
	
	// End of terrain generation
	
	// Create Model and upload to GPU:

	Model* model = LoadDataToModel(
			vertexArray,
			normalArray,
			texCoordArray,
			NULL,
			indexArray,
			vertexCount,
			triangleCount*3);

	return model;
}
