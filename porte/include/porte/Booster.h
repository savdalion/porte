#pragma once


namespace porte {

class Booster;

}


namespace porte {


/**
* Класс для эффективной обработки данных на параллельными алгоритмами
* (OpenCL, Cuda).
* Классы-потомки сами должны определить методы для обмена своих данных
* с "эффективными структурами".
*
* # Название ускоряющей обработку структуры должно быть "booster_t".
*/
class Booster {
public:
    /**
    * Ссылки.
    */
    typedef std::shared_ptr< Booster >  Ptr;
    typedef std::unique_ptr< Booster >  UPtr;



public:
    /**
    * Метод для записи данных "ускоряемого объекта" в спецструктуру.
    */
    virtual void toBooster() = 0;


    /**
    * Метод для записи данных из спецструктуры в "ускоряемый объект".
    */
    virtual void fromBooster() = 0;

};



} // porte

