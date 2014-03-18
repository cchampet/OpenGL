#if defined(VERTEX)

in vec2 VertexPosition;

out vec2 uv;

void main(void)
{   
    uv = VertexPosition * 0.5 + 0.5;
    gl_Position = vec4(VertexPosition.xy, 0.0, 1.0);
}

#endif

#if defined(FRAGMENT)
 
uniform vec3   Resolution;    // Viewport Resolution (in pixels)
uniform float  Time;       // Shader playback time (in seconds)

out vec4 Color;

float uDistanceFactor = 250.0;
float uStarSize = 9.0;
const int uNumStars = 100;
vec4 uDominantColor = vec4(0.8,0.5,0.4,1.0);

float rand(vec2 co){
  return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

vec4 getColor(vec2 seed)
{
   vec4 color;
   
   color.r = uDominantColor.r * rand(seed*134.23);
   color.g = uDominantColor.g * rand(seed*235.43);
   color.b = uDominantColor.b * rand(seed*-32.89);
   color.a = 1.;
   
   return color;
}

float makeStar(float dist, float speed, float size)
{
   vec2 currPos = Resolution.xy * 0.5;
   currPos.x += dist * sin(Time*speed);
   currPos.y += dist * cos(Time*speed);
   return size / (1.0+length(currPos - gl_FragCoord.xy)); 
}

void main(void)
{
   vec4 final = vec4(0);
   
   vec2 derpseed = vec2(3.44,9.12);
   
   for(int i = 0; i < uNumStars; i++) {
      vec4 color = getColor(derpseed);
      float dist = rand(derpseed) * uDistanceFactor;
      float speed = rand(derpseed*3.99);
      float size = rand(derpseed*225.22) * uStarSize;
      final += color * makeStar(dist,speed,size);
           
      derpseed.x += 0.54;
      derpseed.y += 0.24;
   }
   
   final.a = 1.0;
   Color = final;
}

#endif