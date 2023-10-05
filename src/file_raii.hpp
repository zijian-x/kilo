
    using namespace file;
void read_file(const char* filename)
{

    auto fp = file_raii(filename);
    ed_state.filename() = filename;

    char* buf = nullptr;
    while ((buf = fp.nextline()) != nullptr || !fp.eof()) {
        auto line = str(std::move(buf)).remove_newline();
        ed_state.content().push_back(std::move(line));
    }
}

