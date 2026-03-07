
// int main(int argc, char **argv)
// {
// 	int i = 2;
// 	if (argc == 1)
// 	{
// 		std::cout << "Use: ./expr \"arithmetic expression\" \"variables(optional)\"\n";
// 		return (1);
// 	}
// 	Node *tree = parse_expr(argv[1]);
// 	if (!tree)
// 		return (1);
// 	std::map<std::string, int> vars;
// 	if (argc > 2)
// 	{
// 		while (i < argc)
// 			parse_vars(argv[i++], vars);
// 	}
// 	std::cout << "Value: " << eval_tree(tree, vars) << std::endl;
// 	// for(auto& v : vars)
// 	// 	std::cout << v.first << ": " << v.second << "\n";
// 	destroy_tree(tree);
// 	return 0;
// }