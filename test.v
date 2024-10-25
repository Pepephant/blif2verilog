module sample(clk, rst, a, b, c, d, e, f, o, p, q);
input clk, rst;
output o;
output p;
output q;
input a;
input b;
input c;
input d;
input e;
input f;

wire a;
wire b;
wire c;
wire d;
wire e;
wire f;
wire o;
wire p;
wire q;
wire g;
wire h;
wire i;
wire j;
wire k;
wire l;
wire m;
wire n;

assign g = (a) | (d);
assign h = (a & c);
assign i = (!c);
assign j = (d&a) | (e&) | (f&);
assign k = (g) | (h) | (i);
assign l = (h & i & j);
assign m = (i & j);
assign n = (l & m);
assign o = (b & h & k);
assign p = (!g);
assign q = (!n);

endmodule
