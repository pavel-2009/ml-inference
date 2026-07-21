// Базовый класс для работы с моделью
#include <string>


class IModel {
    private:
        std::string name; // Имя модели

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