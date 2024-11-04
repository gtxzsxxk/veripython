module full_adder(y,carryout,a,b,carryin);
  output y, carryout;
  input a,b,carryin;

  assign y = a ^ b ^ carryin;
  assign carryout = ( a & b ) | ( a & carryin ) | ( b & carryin );

endmodule
