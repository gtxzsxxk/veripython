// 1位全加器模块
module full_adder(
    y,          /* 输出 */
    carryout,   /* 进位输出 */
    a,
    b,
    carryin     /* 进位输入 */
);
  output y, carryout;
  input a,b,carryin;

/*
 * assign y = a ^ b ^ carryin;
 * 组合逻辑赋值语句
 */

  assign y = a ^ b ^ carryin;
  assign carryout = ( a & b ) | ( a & carryin ) | ( b & carryin );

endmodule
