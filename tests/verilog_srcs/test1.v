module jfulladder(y,carryout,a,b,carryin);
  output y, carryout;
  input a,b,carryin;

  assign y = a ^ b ^ carryin;
  assign carryout = ( a & b ) | ( a & carryin ) | ( b & carryin );

endmodule

module jripplecarryadder(
  output [3:0] Y,
  output [3:0] carryout,
  input [3:0] A,
  input [3:0] B,
  input carryin
);

  wire c1,c2,c3,c4,c5,c6,c7;

  jfulladder jfa0(Y[0],c1,A[0],B[0],carryin);
  jfulladder jfa1(Y[1],c2,A[1],B[1],c1);
  jfulladder jfa2(Y[2],c3,A[2],B[2],c2);
  jfulladder jfa3(Y[3],carryout,A[3],B[3],c3);

endmodule
