#if defined(VERTEX)

in vec2 VertexPosition;
in vec2 VertexTexCoord;

out vec2 uv;

void main(void)
{   
	uv = VertexPosition * 0.5 + 0.5;
    gl_Position = vec4(VertexPosition.xy, 0.0, 1.0);
}

#endif

#if defined(FRAGMENT)

uniform sampler2D Texture1;
uniform float SobelCoef;

in vec2 uv;

out vec4  OutColor;

uniform mat3 G[2] = mat3[](
        mat3( 
        	1.0, 2.0, 1.0, 
        	0.0, 0.0, 0.0, 
        	-1.0, -2.0, -1.0 ),
        mat3( 
        	1.0, 0.0, -1.0, 
        	2.0, 0.0, -2.0, 
        	1.0, 0.0, -1.0 )
);


void main(void)
{
	mat3 I;
    for (int i=0; i<3; i++) {
	    for (int j=0; j<3; j++) {
	    	// On récupère la couleur d'un pixel précis dans Texture1
	        vec3 sample  = texelFetch(Texture1, ivec2(gl_FragCoord) + ivec2(i-1,j-1), 0 ).rgb;
	        I[i][j] = length(sample); 
	    }
	}

	// Décrire les vecteurs de l'opérateur
	mat3 sx = mat3( 
		1.0, 2.0, 1.0, 
		0.0, 0.0, 0.0, 
		-1.0, -2.0, -1.0 
		);
	mat3 sy = mat3( 
		1.0, 0.0, -1.0, 
		2.0, 0.0, -2.0, 
		1.0, 0.0, -1.0 
		);

	float gx = dot(sx[0], I[0]) + dot(sx[1], I[1]) + dot(sx[2], I[2]); 
	float gy = dot(sy[0], I[0]) + dot(sy[1], I[1]) + dot(sy[2], I[2]);

	float g = sqrt(pow(gx, 2.0)+pow(gy, 2.0));

	vec3 diffuse = texture(Texture1, uv).rgb;

    OutColor = vec4(diffuse - vec3(g), 1.0);
}

#endif
