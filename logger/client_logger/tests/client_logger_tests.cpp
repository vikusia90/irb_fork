#include <gtest/gtest.h>
#include <client_logger.h>
#include <client_logger_builder.h>
#include <windows.h>

int main()
{
    client_logger_builder* b1 = new client_logger_builder;

    logger* l1 = b1->add_console_stream(logger::severity::trace)
            ->add_console_stream(logger::severity::information)
            ->set_format("%d %t %s %m")
            ->build();

    if (l1 != nullptr) {
        l1->log("zhopa", logger::severity::information);
    } else {
        std::cerr << "Ошибка при создании логгера." << std::endl;
    }

    delete l1;

    client_logger_builder* b2 = new client_logger_builder;

    delete b1;

    logger* l2 = b2
            ->transform_with_configuration("C:/Users/biryu/irb_fork/logger/client_logger/tests/conf_logger.json", "file_paths")
            ->build();

    l2->log("pizda228", logger::severity::trace)
            ->log("оалоыд", logger::severity::debug);

    delete l2;


    logger * l3 = b2 ->add_file_stream("C:/Users/biryu/irb_fork/logger/client_logger/tests/1.txt", logger::severity::trace)
            ->add_file_stream("C:/Users/biryu/irb_fork/logger/client_logger/tests/1.txt", logger::severity::critical)
            //->set_format("%d %t aaaaaaaa %s %m")
            -> build();
    l3->log("adfghj", logger::severity::trace);

    delete b2;
    delete l3;

    return 0;


}
