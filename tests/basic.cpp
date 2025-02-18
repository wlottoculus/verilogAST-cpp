#include "common.cpp"
#include "gtest/gtest.h"
#include "verilogAST.hpp"

namespace vAST = verilogAST;

namespace {

TEST(BasicTests, TestNumericLiteral) {
  vAST::NumericLiteral n0("23", 16, false, vAST::DECIMAL);
  EXPECT_EQ(n0.toString(), "16'23");

  vAST::NumericLiteral n1("DEADBEEF", 32, false, vAST::HEX);
  EXPECT_EQ(n1.toString(), "'hDEADBEEF");

  vAST::NumericLiteral n2("011001", 6, false, vAST::BINARY);
  EXPECT_EQ(n2.toString(), "6'b011001");

  vAST::NumericLiteral n3("764", 24, false, vAST::OCTAL);
  EXPECT_EQ(n3.toString(), "24'o764");

  vAST::NumericLiteral n4("764", 8, false);
  EXPECT_EQ(n4.toString(), "8'764");

  vAST::NumericLiteral n5("764", 8);
  EXPECT_EQ(n5.toString(), "8'764");

  vAST::NumericLiteral n6("764");
  EXPECT_EQ(n6.toString(), "764");

  vAST::NumericLiteral n7("764", 8, true);
  EXPECT_EQ(n7.toString(), "8's764");
}

TEST(BasicTests, TestIdentifier) {
  vAST::Identifier id("x");
  EXPECT_EQ(id.toString(), "x");
}

TEST(BasicTests, TestIdentifierEscaped) {
  vAST::Identifier id("instance[5]");
  EXPECT_EQ(id.toString(), "\\instance[5] ");
}

TEST(BasicTests, TestIdentifierKeyword) {
  vAST::Identifier id("or");
  EXPECT_EQ(id.toString(), "\\or ");
}

TEST(BasicTests, TestString) {
  vAST::String str("mystring");
  EXPECT_EQ(str.toString(), "\"mystring\"");
}

TEST(BasicTests, TestIndex) {
  vAST::Index index(vAST::make_id("x"), vAST::make_num("0"));
  EXPECT_EQ(index.toString(), "x[0]");
}

TEST(BasicTests, TestSlice) {
  vAST::Identifier id("x");
  vAST::Slice slice(vAST::make_id("x"), vAST::make_num("31"),
                    vAST::make_num("0"));
  EXPECT_EQ(slice.toString(), "x[31:0]");
}

TEST(BasicTests, TestVector) {
  vAST::Vector slice(vAST::make_id("x"), vAST::make_num("31"),
                     vAST::make_num("0"));
  EXPECT_EQ(slice.toString(), "[31:0] x");
}

TEST(BasicTests, TestBinaryOp) {
  std::vector<std::pair<vAST::BinOp::BinOp, std::string>> ops;
  ops.push_back(std::make_pair(vAST::BinOp::LSHIFT, "<<"));
  ops.push_back(std::make_pair(vAST::BinOp::RSHIFT, ">>"));
  ops.push_back(std::make_pair(vAST::BinOp::AND, "&&"));
  ops.push_back(std::make_pair(vAST::BinOp::OR, "||"));
  ops.push_back(std::make_pair(vAST::BinOp::EQ, "=="));
  ops.push_back(std::make_pair(vAST::BinOp::NEQ, "!="));
  ops.push_back(std::make_pair(vAST::BinOp::ADD, "+"));
  ops.push_back(std::make_pair(vAST::BinOp::SUB, "-"));
  ops.push_back(std::make_pair(vAST::BinOp::MUL, "*"));
  ops.push_back(std::make_pair(vAST::BinOp::DIV, "/"));
  ops.push_back(std::make_pair(vAST::BinOp::POW, "**"));
  ops.push_back(std::make_pair(vAST::BinOp::MOD, "%"));
  ops.push_back(std::make_pair(vAST::BinOp::ALSHIFT, "<<<"));
  ops.push_back(std::make_pair(vAST::BinOp::ARSHIFT, ">>>"));
  for (auto it : ops) {
    vAST::BinOp::BinOp op = it.first;
    std::string op_str = it.second;
    vAST::BinaryOp bin_op(vAST::make_id("x"), op, vAST::make_id("y"));
    EXPECT_EQ(bin_op.toString(), "x " + op_str + " y");
  }
}

TEST(BasicTests, TestUnaryOp) {
  std::vector<std::pair<vAST::UnOp::UnOp, std::string>> ops;
  ops.push_back(std::make_pair(vAST::UnOp::NOT, "!"));
  ops.push_back(std::make_pair(vAST::UnOp::INVERT, "~"));
  ops.push_back(std::make_pair(vAST::UnOp::AND, "&"));
  ops.push_back(std::make_pair(vAST::UnOp::NAND, "~&"));
  ops.push_back(std::make_pair(vAST::UnOp::OR, "|"));
  ops.push_back(std::make_pair(vAST::UnOp::NOR, "~|"));
  ops.push_back(std::make_pair(vAST::UnOp::XOR, "^"));
  ops.push_back(std::make_pair(vAST::UnOp::NXOR, "~^"));
  ops.push_back(std::make_pair(vAST::UnOp::XNOR, "^~"));
  ops.push_back(std::make_pair(vAST::UnOp::PLUS, "+"));
  ops.push_back(std::make_pair(vAST::UnOp::MINUS, "-"));
  for (auto it : ops) {
    vAST::UnOp::UnOp op = it.first;
    std::string op_str = it.second;
    vAST::UnaryOp un_op(vAST::make_id("x"), op);
    EXPECT_EQ(un_op.toString(), op_str + " x");
  }
}

TEST(BasicTests, TestTernaryOp) {
  vAST::UnaryOp un_op(std::make_unique<vAST::Identifier>("x"),
                      vAST::UnOp::INVERT);
  vAST::TernaryOp tern_op(
      std::make_unique<vAST::UnaryOp>(vAST::make_id("x"), vAST::UnOp::INVERT),
      vAST::make_num("1"), vAST::make_num("0"));
  EXPECT_EQ(tern_op.toString(), "~ x ? 1 : 0");
}

TEST(BasicTests, TestConcat) {
  std::vector<std::unique_ptr<vAST::Expression>> args;
  args.push_back(vAST::make_id("x"));
  args.push_back(vAST::make_id("y"));
  vAST::Concat concat(std::move(args));
  EXPECT_EQ(concat.toString(), "{x,y}");
}

TEST(BasicTests, TestNegEdge) {
  vAST::NegEdge neg_edge(vAST::make_id("clk"));

  EXPECT_EQ(neg_edge.toString(), "negedge clk");
}

TEST(BasicTests, TestPosEdge) {
  vAST::PosEdge pos_edge(vAST::make_id("clk"));

  EXPECT_EQ(pos_edge.toString(), "posedge clk");
}

TEST(BasicTests, TestPort) {
  vAST::Port i_port(vAST::make_id("i"), vAST::INPUT, vAST::WIRE);

  EXPECT_EQ(i_port.toString(), "input i");

  vAST::Port o_port(vAST::make_id("o"), vAST::OUTPUT, vAST::WIRE);

  EXPECT_EQ(o_port.toString(), "output o");

  vAST::Port io_port(vAST::make_id("io"), vAST::INOUT, vAST::WIRE);

  EXPECT_EQ(io_port.toString(), "inout io");

  vAST::Port o_reg_port(vAST::make_id("o"), vAST::OUTPUT, vAST::REG);

  EXPECT_EQ(o_reg_port.toString(), "output reg o");
}

TEST(BasicTests, TestStringPort) {
  vAST::StringPort port("output reg [width-1:0] I");

  EXPECT_EQ(port.toString(), "output reg [width-1:0] I");
}

TEST(BasicTests, TestModuleInst) {
  std::string module_name = "test_module";

  vAST::Parameters parameters = make_simple_params();

  std::string instance_name = "test_module_inst";

  vAST::ModuleInstantiation module_inst(module_name, std::move(parameters),
                                        instance_name,
                                        make_simple_connections());

  EXPECT_EQ(module_inst.toString(),
            "test_module #(.param0(0), .param1(1)) "
            "test_module_inst(.a(a), .b(b[0]), .c(c[31:0]));");
}

TEST(BasicTests, TestModule) {
  std::string name = "test_module";

  vAST::Parameters parameters;
  vAST::Module module(name, make_simple_ports(), make_simple_body(),
                      std::move(parameters));

  std::string expected_str =
      "module test_module (input i, output o);\nother_module #(.param0(0), "
      ".param1(1)) other_module_inst(.a(a), .b(b[0]), "
      ".c(c[31:0]));\nendmodule\n";
  EXPECT_EQ(module.toString(), expected_str);
}

TEST(BasicTests, TestParamModule) {
  std::string name = "test_module";

  vAST::Module module_with_params(name, make_simple_ports(), make_simple_body(),
                                  make_simple_params());

  std::string expected_str =
      "module test_module #(parameter param0 = 0, parameter param1 = "
      "1) (input i, output o);\nother_module #(.param0(0), "
      ".param1(1)) other_module_inst(.a(a), .b(b[0]), "
      ".c(c[31:0]));\nendmodule\n";
  EXPECT_EQ(module_with_params.toString(), expected_str);
}

TEST(BasicTests, TestStringBodyModule) {
  std::string name = "test_module";

  std::string module_name = "other_module";

  std::string string_body = "reg d;\nassign d = a + b;\nassign c = d;";
  vAST::StringBodyModule string_body_module(name, make_simple_ports(),
                                            string_body, make_simple_params());
  std::string expected_str =
      "module test_module #(parameter param0 = 0, parameter param1 = "
      "1) (input i, output o);\nreg d;\nassign d = a + b;\nassign c = "
      "d;\nendmodule\n";
  EXPECT_EQ(string_body_module.toString(), expected_str);

  vAST::StringModule string_module(expected_str);
  EXPECT_EQ(string_module.toString(), expected_str);
}

TEST(BasicTests, TestDeclaration) {
  vAST::Wire wire(std::make_unique<vAST::Identifier>("a"));
  EXPECT_EQ(wire.toString(), "wire a;");

  vAST::Reg reg(std::make_unique<vAST::Identifier>("a"));
  EXPECT_EQ(reg.toString(), "reg a;");

  vAST::Reg reg_slice(std::make_unique<vAST::Slice>(
      std::make_unique<vAST::Identifier>("x"),
      std::make_unique<vAST::NumericLiteral>("31"),
      std::make_unique<vAST::NumericLiteral>("0")));
  EXPECT_EQ(reg_slice.toString(), "reg x[31:0];");

  vAST::Reg reg_index(std::make_unique<vAST::Index>(
      std::make_unique<vAST::Identifier>("x"),
      std::make_unique<vAST::NumericLiteral>("31")));
  EXPECT_EQ(reg_index.toString(), "reg x[31];");

  vAST::Reg reg_vec(std::make_unique<vAST::Vector>(
      std::make_unique<vAST::Identifier>("x"),
      std::make_unique<vAST::NumericLiteral>("31"),
      std::make_unique<vAST::NumericLiteral>("0")));
  EXPECT_EQ(reg_vec.toString(), "reg [31:0] x;");
}

TEST(BasicTests, TestAssign) {
  vAST::ContinuousAssign cont_assign(std::make_unique<vAST::Identifier>("a"),
                                     std::make_unique<vAST::Identifier>("b"));
  EXPECT_EQ(cont_assign.toString(), "assign a = b;");

  vAST::BlockingAssign blocking_assign(std::make_unique<vAST::Identifier>("a"),
                                       std::make_unique<vAST::Identifier>("b"));
  EXPECT_EQ(blocking_assign.toString(), "a = b;");

  vAST::NonBlockingAssign non_blocking_assign(
      std::make_unique<vAST::Identifier>("a"),
      std::make_unique<vAST::Identifier>("b"));
  EXPECT_EQ(non_blocking_assign.toString(), "a <= b;");
}

TEST(BasicTests, TestAlways) {
  std::vector<std::variant<
      std::unique_ptr<vAST::Identifier>, std::unique_ptr<vAST::PosEdge>,
      std::unique_ptr<vAST::NegEdge>, std::unique_ptr<vAST::Star>>>
      sensitivity_list;
  sensitivity_list.push_back(std::make_unique<vAST::Identifier>("a"));
  sensitivity_list.push_back(
      std::make_unique<vAST::PosEdge>(std::make_unique<vAST::Identifier>("b")));
  sensitivity_list.push_back(
      std::make_unique<vAST::NegEdge>(std::make_unique<vAST::Identifier>("c")));
  vAST::Always always(std::move(sensitivity_list), make_simple_always_body());
  std::string expected_str =
      "always @(a, posedge b, negedge c) begin\n"
      "a = b;\n"
      "b <= c;\n"
      "end\n";
  EXPECT_EQ(always.toString(), expected_str);
}

TEST(BasicTests, TestAlwaysStar) {
  std::vector<std::variant<
      std::unique_ptr<vAST::Identifier>, std::unique_ptr<vAST::PosEdge>,
      std::unique_ptr<vAST::NegEdge>, std::unique_ptr<vAST::Star>>>
      sensitivity_list;
  sensitivity_list.push_back(std::make_unique<vAST::Star>());
  vAST::Always always_star(std::move(sensitivity_list),
                           make_simple_always_body());
  std::string expected_str =
      "always @(*) begin\n"
      "a = b;\n"
      "b <= c;\n"
      "end\n";
  EXPECT_EQ(always_star.toString(), expected_str);
}

TEST(BasicTests, TestAlwaysEmpty) {
  std::vector<std::variant<
      std::unique_ptr<vAST::Identifier>, std::unique_ptr<vAST::PosEdge>,
      std::unique_ptr<vAST::NegEdge>, std::unique_ptr<vAST::Star>>>
      sensitivity_list;
  std::vector<std::variant<std::unique_ptr<vAST::BehavioralStatement>,
                           std::unique_ptr<vAST::Declaration>>>
      body;

  ASSERT_THROW(
      vAST::Always always_empty(std::move(sensitivity_list), std::move(body)),
      std::runtime_error);
}

TEST(BasicTests, File) {
  std::vector<std::unique_ptr<vAST::AbstractModule>> modules;
  vAST::Parameters parameters0;
  modules.push_back(std::make_unique<vAST::Module>(
      "test_module0", make_simple_ports(), make_simple_body(),
      std::move(parameters0)));

  modules.push_back(
      std::make_unique<vAST::Module>("test_module1", make_simple_ports(),
                                     make_simple_body(), make_simple_params()));

  vAST::File file(modules);

  std::string expected_str =
      "module test_module0 (input i, output o);\nother_module "
      "#(.param0(0), .param1(1)) other_module_inst(.a(a), "
      ".b(b[0]), .c(c[31:0]));\nendmodule\n\n"
      "module test_module1 #(parameter param0 = 0, parameter param1 = "
      "1) (input i, output o);\nother_module #(.param0(0), "
      ".param1(1)) other_module_inst(.a(a), .b(b[0]), "
      ".c(c[31:0]));\nendmodule\n";
  EXPECT_EQ(file.toString(), expected_str);
}
TEST(BasicTests, Comment) {
  vAST::SingleLineComment single_line_comment("Test comment");
  EXPECT_EQ(single_line_comment.toString(), "// Test comment");
  vAST::BlockComment block_comment("Test comment\non multiple lines");
  EXPECT_EQ(block_comment.toString(),
            "/*\nTest comment\non multiple lines\n*/");
}

}  // namespace

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
