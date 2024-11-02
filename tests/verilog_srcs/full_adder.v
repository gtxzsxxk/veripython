module full_adder(y,carryout,a,b,carryin);
  output y, carryout;
  input a,b,carryin;

  wire test = a ^ b;
  wire test2;

  assign test2 = ( a & b ) | ( a & carryin ) | ( b & carryin );

  assign y = test ^ carryin;
  assign carryout = test2;

endmodule
