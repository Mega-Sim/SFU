#include <iostream>
#define  CSV_IO_NO_THREAD
#include "csv.h"

int main(){
    std::string path = "../test/map/data/cid.tsv";
    io::CSVReader<2, io::trim_chars<' ', '\t'>, io::no_quote_escape<'\t'>> in(path);
    in.read_header(io::ignore_extra_column, "LayoutID", "ResetNode");
    std::string layout; int node;
    while(in.read_row(layout, node)){
        std::cout << layout << "," << node << std::endl;
    }
    return 0;
}
