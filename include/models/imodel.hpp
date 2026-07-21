// Базовый класс для работы с моделью
#include <string>


class IModel {
    public:

        std::string id; // Уникальный идентификатор модели

        std::string name; // Название модели

        std::string version; // Версия модели

        std::string author; // Автор модели

        std::string description; // Описание модели

        std::string input_type; // Тип входных данных модели

        size_t input_size; // Размер входных данных модели

        std::string output_type; // Тип выходных данных модели

    public:
        // Конструктор
        IModel(const std::string& modelName) : name(modelName) {}

        // Виртуальный деструктор
        virtual ~IModel() = default;

        // Метод для получения имени модели
        std::string getName() const {
            return name;
        }

        // Чисто виртуальная функция для предсказания на основе входных данных
        virtual void predict() = 0;
};