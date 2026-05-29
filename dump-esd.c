#include "esd.c"

void print_usage(const char *program_name) {
	printf("USAGE:\n");
	printf("\t%s [esd_path] <state_group_id>", program_name);
	printf("\n");
}

int main(int argc, const char **argv) {
	const char *program_name = shift(argv, argc); // 
	if (argc == 0) {
		nob_log(NOB_ERROR, "missing esd file");
		print_usage(program_name);
		return 1;
	}
	const char *esd_path = shift(argv, argc);
	int sg_id = -1;
	if (argc > 0) {
		sg_id = atoi(shift(argv, argc));
	}
	String_Builder sb = {0};
	if (!read_entire_file(esd_path, &sb)) return 1;
	ESD esd = read_esd(sb_to_sv(sb));
	da_foreach(StateGroup, sg, &esd.state_groups) {
		if (sg_id == -1) {
			print_state_group(&esd, sg);
		} else if (sg->id == sg_id) {
			print_state_group(&esd, sg);
			break;
		}
	}
	free_esd(&esd);
	da_free(sb);
	return 0;
}
