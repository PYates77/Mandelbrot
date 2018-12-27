#version 410

out vec4 colorOut;
uniform double screen_ratio;
uniform dvec2 screen_size;
uniform dvec2 center;
uniform double zoom;
uniform int itr;
uniform dvec2 constant;
uniform int exponent;

vec4 map_to_color(float t) {
    float r = 9.0 * (1.0 - t) * t * t * t;
    float g = 15.0 * (1.0 - t) * (1.0 - t) * t * t;
    float b = 8.5 * (1.0 - t) * (1.0 - t) * (1.0 - t) * t;

    return vec4(r, g, b, 1.0);
}
dvec2 pow(dvec2 num, int exp){
    int j;
    dvec2 ans;
    ans.x = 1;
    ans.y = 0;
    for(j=0; j<exp; j++){
        double x = ans.x * num.x - ans.y * num.y;
        double y = ans.y * num.x + ans.x * num.y;
        ans.x = x;
        ans. y = y;
    }
    return ans;
}
void main()
{
    dvec2 z, c;
    z.x = screen_ratio * ( gl_FragCoord.x / screen_size.x  - 0.5);
    z.y = (gl_FragCoord.y / screen_size.y - 0.5);

    z.x /= zoom;
    z.y /= zoom;

    z.x += center.x;
    z.y += center.y;

    c.x = constant.x;
    c.y = constant.y;
    int i;
    for(i = 0; i < itr; i++) {
        dvec2 p = pow(z, int(exponent));

        double x = p.x + c.x; 
        double y = p.y + c.y;

		if((x * x + y * y) > 4.0) break;
		z.x = x;
		z.y = y;
    }

    double t = double(i) / double(itr);

    colorOut = map_to_color(float(t));
}


