int driver(int argc, char** argv);
void* __stack_chk_guard = (void*) 8720484132186315953;

int main(int argc, char** argv)
{
    driver(argc, argv);
    return 0;
}