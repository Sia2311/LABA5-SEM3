#include <iostream>
#include <string>
#include <limits>
#include <vector>
#include <fstream>

using namespace std;
/** @brief Базовый класс пользователя */
class User{
    protected:
    int id;
    string name;

    public:
    //конструктор юзера
    User(int id_, const string& name_) : id(id_), name(name_){}
    //так как будут потомки юзаем виртуальный деструктор
    virtual ~User() = default;

    int getId() const{
        return id;
    }
    const string& getName() const{
        return name;
    }
    //роль пользователя студент или препод (виртуал так как у юзер нет роли а у налсдеников есть)
    virtual string getRole () const = 0;

    virtual void printInfo() const{
        cout << "id: " << id << ", имя: " << name;
    }
};
/** @brief Студент */
class Student : public User{
    private:
    string group;

    public:
    //конструктор студента
    Student(int id_, const string& name_ , const string& group_) : User(id_, name_), group(group_){}
    //переопределяем роль
    string getRole() const override {
        return "студент";
    }
    const string& getGroup() const{
        return group;
    }
    //переопределяем метод инфы
    void printInfo() const override{
        cout << "[студент] ";
        User::printInfo();
        cout << ", группа " << group;
    }

};
/** @brief Преподаватель */
class Teacher : public User{
    public:
    Teacher(int id_, const string& name_) : User(id_, name_){}

    string getRole() const override {
        return "препод";
    }
    void printInfo() const override{
        cout << "[препод] ";
        User::printInfo();
    }
};

enum class WorkType {
    Report, //доклад
    Lab
};
/** @brief Работа */
class Work{
    protected:
    int id;
    string title; //название задания

    public:
    Work(int id_, const string& title_) : id(id_), title(title_){}

    virtual ~Work() = default;

    int getId() const{
        return id;
    }
    const string& getTitle() const{
        return title;
    }
    //функции которые должны будут реализовать наследники
    virtual WorkType getType() const = 0;
    virtual string getTypeName() const = 0;
    //а эту переопределят
    virtual void printInfo() const{
        cout << "работа #" << id << " \"" << title << "\"";
    }
};
/** @brief Доклад */
class ReportWork : public Work{
    public:
    ReportWork(int id_, const string& title_) : Work(id_, title_){}

    WorkType getType() const override{
        return WorkType::Report;
    }
    string getTypeName() const override{
        return "доклад";
    }
    //переопределим инфу
    void printInfo() const override{
        cout << "[доклад] ";
        Work::printInfo();
    }
};
/** @brief Лабораторная */
class LabWork : public Work{
    public:
    LabWork(int id_, const string& title_) : Work(id_, title_){}

    WorkType getType() const override{
        return WorkType::Lab;
    }
    string getTypeName() const override{
        return "лаба";
    }
    void printInfo() const override{
        cout << "[лаба] ";
        Work::printInfo();
    }
};

/** @brief Фабрика работ */
class WorkFactory {
    public:
    //создаем объект нужного типа 
    static Work* createWork(WorkType type, int id, const string& title){
        //если доклад
        if(type == WorkType::Report){
            return new ReportWork(id, title);
        }
        //лаба
        if(type == WorkType::Lab){
            return new LabWork(id, title);
        }
        return nullptr;
    }
};

/** @brief Слот задания */
struct AssignmentSlot{
    Work* work;
    Student* reservedBy; //студент который записался
    bool submitted; //студент сказл, что сдал
    bool approved; //препод подтвердил
    int grade; //оценка

    AssignmentSlot(Work* w) : work(w), reservedBy(nullptr), submitted(false), approved(false), grade(0){}

    void print() const {
        if (work) {
            cout << "    задание #" << work->getId()
                 << " (" << work->getTypeName() << "): "
                 << work->getTitle() << "\n";
        } else {
            cout << "    (пустой слот без работы)\n";
            return;
        }
        //никто не записан
        if (!reservedBy) {
            cout << "      слот свободен\n";
            return;
        }

        // если студент есть — выводим базовую инфу
        cout << "      студент: " << reservedBy->getName()
             << " (группа: " << reservedBy->getGroup() << ")\n";

        // дальше выводим статус
        if (!submitted) {
            cout << "      статус: записан, но ещё не отметил сдачу\n";
        } else if (submitted && !approved) {
            cout << "      статус: сдано, ожидает проверки преподавателя\n";
        } else { // submitted && approved
            cout << "      статус: утверждено, оценка: " << grade << "\n";
        }
    }
};


/** @brief Предмет */
class Subject {
    private:
    int id;
    string name;
    Teacher* owner; //указатель на препода, который ведёт предмет

    vector<Student*> students;
    vector<AssignmentSlot> assigments;

    public:
    Subject (int id_, const string& name_, Teacher* owner_) : id(id_), name(name_), owner(owner_){}

    int getId() const{
        return id;
    }
    const string& getName() const{
        return name;
    }
    Teacher* getOwner() const{
        return owner;
    }
    //инвайт студента на предмет
    void addStudent(Student* student) {
        if (!student) {
            cout << "ошибка: нет студента\n";
            return;
        }
    
        // Проверка на дубликаты
        for (auto* s : students) {
            if (s == student) {
                cout << "студент уже записан на предмет\n";
                return;
            }
        }
    
        students.push_back(student);
        cout << "студент добавлен на предмет\n";
    }
    
    //добавить задание
    void addWork(WorkType type, int id, const string& title){
        Work* w = WorkFactory::createWork(type, id, title); //связь с фабрикой
        assigments.emplace_back(w); // создаём слот для этой работы
    }
    //краткий вывод
    void printShort() const{
        cout << "предмет #" << id << " \"" << name << "\"";

        if(owner){
            cout << ", препод: " << owner -> getName();
        }
        cout << endl;
    }
    //вывод полной инфы
    void printFull() const {
        printShort();
    
        cout << "  студенты (" << students.size() << "):\n";
        for (Student* st : students) {
            cout << "    - ";
            if (st) {
                st->printInfo();
            } else {
                cout << "(null студент)";
            }
            cout << "\n";
        }
    
        cout << "  задания (" << assigments.size() << "):\n";
        for (const AssignmentSlot& slot : assigments) {
            slot.print(); // у слота свой красивый вывод
        }
    }

    // геттеры для списка студентов и заданий (для активности студента)
    const vector<Student*>& getStudentsList() const {
        return students;
    }

    const vector<AssignmentSlot>& getAssignmentsList() const {
        return assigments;
    }

    //студент записался на задание по айди
    bool reserveWork(int workId, Student* student){
        for(auto& slot : assigments){
            if(slot.work && slot.work -> getId() == workId){
                if(slot.reservedBy != nullptr){
                    cout << "Слот уже занят другим студентом" << endl;
                    return false;
                }
                slot.reservedBy = student;
                slot.submitted = false;
                slot.approved = false;
                slot.grade = 0;
                cout << "студент " << student -> getName() << " записаля на задание #" << workId << endl;
                return true;
            }
        }
        cout << "задание с id " << workId << " не найдено в этом предмете" << endl;
        return false;
    }
    //студент отмечает, что сдал
    bool markSubmitted(int workId, Student* student){
        for(auto& slot : assigments){
            if(slot.work && slot.work -> getId() == workId){
                if(slot.reservedBy != student){
                    cout <<"это задание не занятом этим студентом" << endl;
                    return false;
                }
                slot.submitted = true;
                cout << "студент " << student -> getName() << " отметил, что сдал задание #" << workId << endl;
                return true;
            }
        }
        cout << "задание с id " << workId << " не найдено" << endl;
        return false;
    }
    //препод утверждает сдачу и ставит оценку
    bool approveWork(int workId, int grade) {
        for (auto& slot : assigments){
            if(slot.work && slot.work -> getId() == workId){
                if(slot.reservedBy == nullptr){
                    cout << "на данное задание никто не записан" << endl;
                    return false;
                }
                if(!slot.submitted){
                    cout << "студент ещё не отметил сдачу" << endl;
                    return false;
                }
                slot.approved = true;
                slot.grade = grade;
                cout << "сдача задания #" << workId << " утверждена, оценка: " << grade << endl;
                return true;
            }
        } 
        cout << "задание с id " << workId << " не найдено" << endl;
        return false;
    }
      // преподаватель отклоняет сдачу, слот очищается
      bool rejectWork(int workId) {
        for (auto& slot : assigments) {
            if (slot.work && slot.work->getId() == workId) {
                if (slot.reservedBy == nullptr) {
                    cout << "на это задание никто не записан\n";
                    return false;
                }
                cout << "сдача задания #" << workId << " отклонена, слот освобождён\n";
                slot.reservedBy = nullptr;
                slot.submitted = false;
                slot.approved = false;
                slot.grade = 0;
                return true;
            }
        }
        cout << "задание с id " << workId << " не найдено\n";
        return false;
    }

    // студент сам спрыгивает с задания
    bool dropWork(int workId, Student* student) {
        for (auto& slot : assigments) {
            if (slot.work && slot.work->getId() == workId) {
                if (slot.reservedBy != student) {
                    cout << "этим заданием занят не этот студент\n";
                    return false;
                }
                cout << "студент " << student->getName()
                     << " спрыгнул с задания #" << workId << "\n";
                slot.reservedBy = nullptr;
                slot.submitted = false;
                slot.approved = false;
                slot.grade = 0;
                return true;
            }
        }
        cout << "задание с id " << workId << " не найдено\n";
        return false;
    }
    ~Subject() {
        for (auto& slot : assigments) {
            delete slot.work;   // освобождаем каждую работу
        }
    }
    
};

/** @brief Университетская система */
class UniversitySystem {
    private:
        // списки указателей на объекты
        vector<Student*> students;
        vector<Teacher*> teachers;
        vector<Subject*> subjects;
    
        int nextUserId = 1;      // следующий id для пользователя
        int nextSubjectId = 1;   // следующий id для предмета
        int nextWorkId = 1;      // следующий id для работы
    
    public:
        // добавление преподавателя
        void addTeacher() {
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // чистим буфер
            string name;
            cout << "введите имя преподавателя: ";
            getline(cin, name);
    
            Teacher* t = new Teacher(nextUserId++, name);
            teachers.push_back(t);
    
            cout << "преподаватель добавлен, id = " << t->getId() << "\n";
        }
    
        // добавление студента
        void addStudent() {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            string name;
            string group;
    
            cout << "введите имя студента: ";
            getline(cin, name);
            cout << "введите группу: ";
            getline(cin, group);
    
            Student* s = new Student(nextUserId++, name, group);
            students.push_back(s);
    
            cout << "студент добавлен, id = " << s->getId() << "\n";
        }
    
        // найти преподавателя по id
        Teacher* findTeacherById(int id) {
            for (auto* t : teachers) {
                if (t->getId() == id) return t;
            }
            return nullptr;
        }
    
        // найти студента по id
        Student* findStudentById(int id) {
            for (auto* s : students) {
                if (s->getId() == id) return s;
            }
            return nullptr;
        }
    
        // найти предмет по id
        Subject* findSubjectById(int id) {
            for (auto* sub : subjects) {
                if (sub->getId() == id) return sub;
            }
            return nullptr;
        }
    
        // создать предмет
        void addSubject() {
            if (teachers.empty()) {
                cout << "сначала добавьте хотя бы одного преподавателя\n";
                return;
            }
    
            cout << "список преподавателей:\n";
            for (auto* t : teachers) {
                cout << "  id " << t->getId() << ": " << t->getName() << "\n";
            }
    
            int teacherId;
            cout << "введите id преподавателя-владельца предмета: ";
            cin >> teacherId;
    
            Teacher* owner = findTeacherById(teacherId);
            if (!owner) {
                cout << "преподаватель с таким id не найден\n";
                return;
            }
    
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            string name;
            cout << "введите название предмета: ";
            getline(cin, name);
    
            Subject* subj = new Subject(nextSubjectId++, name, owner);
            subjects.push_back(subj);
    
            cout << "предмет создан, id = " << subj->getId() << "\n";
        }
    
        // записать студента на предмет (просто добавляем его в список студентов предмета)
        void enrollStudentToSubject() {
            if (subjects.empty() || students.empty()) {
                cout << "нет предметов или студентов\n";
                return;
            }
    
            int subjId, studId;
            cout << "введите id предмета: ";
            cin >> subjId;
            cout << "введите id студента: ";
            cin >> studId;
    
            Subject* subj = findSubjectById(subjId);
            Student* stud = findStudentById(studId);
    
            if (!subj || !stud) {
                cout << "неверный id предмета или студента\n";
                return;
            }
    
            subj->addStudent(stud);
            cout << "студент добавлен на предмет\n";
        }
    
        // добавить задание на предмет (доклад/лаба)
        void addWorkToSubject() {
            if (subjects.empty()) {
                cout << "нет предметов\n";
                return;
            }
    
            int subjId;
            cout << "введите id предмета: ";
            cin >> subjId;
    
            Subject* subj = findSubjectById(subjId);
            if (!subj) {
                cout << "предмет не найден\n";
                return;
            }
    
            int typeInt;
            cout << "выберите тип задания (0 - доклад, 1 - лаба): ";
            cin >> typeInt;
    
            if (typeInt != 0 && typeInt != 1) {
                cout << "неверный тип\n";
                return;
            }
    
            WorkType type = (typeInt == 0) ? WorkType::Report : WorkType::Lab;
    
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            string title;
            cout << "введите название задания: ";
            getline(cin, title);
    
            subj->addWork(type, nextWorkId++, title);
    
            cout << "задание добавлено на предмет\n";
        }
    
        // студент записывается на конкретное задание
        void reserveWorkOnSubject() {
            int subjId, studId, workId;
            cout << "введите id предмета: ";
            cin >> subjId;
            cout << "введите id студента: ";
            cin >> studId;
            cout << "введите id задания: ";
            cin >> workId;
    
            Subject* subj = findSubjectById(subjId);
            Student* stud = findStudentById(studId);
    
            if (!subj || !stud) {
                cout << "неверный id предмета или студента\n";
                return;
            }
    
            subj->reserveWork(workId, stud);
        }
    
        // студент отмечает, что сдал работу
        void studentSubmitWork() {
            int subjId, studId, workId;
            cout << "введите id предмета: ";
            cin >> subjId;
            cout << "введите id студента: ";
            cin >> studId;
            cout << "введите id задания: ";
            cin >> workId;
    
            Subject* subj = findSubjectById(subjId);
            Student* stud = findStudentById(studId);
    
            if (!subj || !stud) {
                cout << "неверный id предмета или студента\n";
                return;
            }
    
            subj->markSubmitted(workId, stud);
        }
    
        // преподаватель утверждает работу и ставит оценку
        void approveWorkOnSubject() {
            int subjId, workId, grade;
            cout << "введите id предмета: ";
            cin >> subjId;
            cout << "введите id задания: ";
            cin >> workId;
            cout << "введите оценку: ";
            cin >> grade;
    
            Subject* subj = findSubjectById(subjId);
            if (!subj) {
                cout << "предмет не найден\n";
                return;
            }
    
            subj->approveWork(workId, grade);
        }
    
        // преподаватель отклоняет работу
        void rejectWorkOnSubject() {
            int subjId, workId;
            cout << "введите id предмета: ";
            cin >> subjId;
            cout << "введите id задания: ";
            cin >> workId;
    
            Subject* subj = findSubjectById(subjId);
            if (!subj) {
                cout << "предмет не найден\n";
                return;
            }
    
            subj->rejectWork(workId);
        }
    
        // студент спрыгивает с задания
        void dropWorkOnSubject() {
            int subjId, studId, workId;
            cout << "введите id предмета: ";
            cin >> subjId;
            cout << "введите id студента: ";
            cin >> studId;
            cout << "введите id задания: ";
            cin >> workId;
    
            Subject* subj = findSubjectById(subjId);
            Student* stud = findStudentById(studId);
    
            if (!subj || !stud) {
                cout << "неверный id предмета или студента\n";
                return;
            }
    
            subj->dropWork(workId, stud);
        }
    
        // вывести список предметов (кратко)
        void listSubjects() const {
            if (subjects.empty()) {
                cout << "нет предметов\n";
                return;
            }
    
            cout << "список предметов:\n";
            for (auto* s : subjects) {
                s->printShort();
            }
        }

        // вывести список всех студентов
        void listStudents() const {
            if (students.empty()) {
                cout << "студентов нет\n";
                return;
            }
            cout << "список студентов:\n";
            for (auto* s : students) {
                cout << "  id " << s->getId()
                     << ": " << s->getName()
                     << " (группа: " << s->getGroup() << ")\n";
            }
        }

        // вывести список всех преподавателей
        void listTeachers() const {
            if (teachers.empty()) {
                cout << "преподавателей нет\n";
                return;
            }
            cout << "список преподавателей:\n";
            for (auto* t : teachers) {
                cout << "  id " << t->getId()
                     << ": " << t->getName()
                     << " (роль: " << t->getRole() << ")\n";
            }
        }

        // вывести преподавателей по предметам
        void listTeachersBySubjects() const {
            if (subjects.empty()) {
                cout << "нет предметов\n";
                return;
            }
            cout << "преподаватели по предметам:\n";
            for (auto* sub : subjects) {
                cout << "  предмет #" << sub->getId()
                     << " \"" << sub->getName() << "\"";
                if (sub->getOwner()) {
                    cout << " → " << sub->getOwner()->getName();
                } else {
                    cout << " → (нет преподавателя)";
                }
                cout << "\n";
            }
        }

        // показать активность студента по всем предметам
        void showStudentActivity() const {
            if (students.empty()) {
                cout << "нет студентов\n";
                return;
            }

            int studId;
            cout << "введите id студента: ";
            cin >> studId;

            Student* stud = nullptr;
            for (auto* s : students) {
                if (s->getId() == studId) {
                    stud = s;
                    break;
                }
            }

            if (!stud) {
                cout << "студент не найден\n";
                return;
            }

            cout << "\n=== активность студента: " << stud->getName() << " ===\n";

            bool foundAny = false;

            // пробегаемся по всем предметам
            for (auto* subj : subjects) {
                bool onThisSubject = false;
                for (auto* s : subj->getStudentsList()) {
                    if (s == stud) {
                        onThisSubject = true;
                        break;
                    }
                }
                if (!onThisSubject) {
                    continue;
                }

                cout << "\nпредмет: " << subj->getName() << "\n";

                for (const auto& slot : subj->getAssignmentsList()) {
                    if (slot.work == nullptr) continue;

                    // если слот свободен – это не "делает студент", пропускаем
                    if (slot.reservedBy != stud) {
                        continue;
                    }

                    foundAny = true;

                    cout << " - " << slot.work->getTypeName()
                         << " \"" << slot.work->getTitle() << "\"";

                    if (!slot.submitted && !slot.approved) {
                        cout << " → записан, ещё не сдал\n";
                    } else if (slot.submitted && !slot.approved) {
                        cout << " → сдано, ожидает проверки\n";
                    } else if (slot.approved) {
                        cout << " → оценка: " << slot.grade << "\n";
                    } else {
                        cout << "\n";
                    }
                }
            }

            if (!foundAny) {
                cout << "у студента нет активных работ\n";
            }
        }
    
        // показать подробную инфу по одному предмету
        void showSubjectDetails() const {
            int subjId;
            cout << "введите id предмета: ";
            cin >> subjId;
    
            for (auto* s : subjects) {
                if (s->getId() == subjId) {
                    s->printFull();
                    return;
                }
            }
            cout << "предмет не найден\n";
        }
    
        //выгрузка итогов
        void exportSubjectReport() const {
            int subjId;
            cout << "введите id предмета для отчёта: ";
            cin >> subjId;
        
            for (auto* s : subjects) {
                if (s->getId() == subjId) {
        
                    // Показываем отчёт в консоли
                    cout << "==== отчёт по предмету \"" << s->getName() << "\" ====\n";
                    s->printFull();
                    cout << "==== конец отчёта ====\n";
        
                    string filename = "report_subject_" + to_string(subjId) + ".txt";
        
                    ofstream out(filename);
                    if (!out) {
                        cout << "ошибка: не удалось открыть файл для записи\n";
                        return;
                    }
        
                    // 4) Записываем данные
                    out << "ОТЧЁТ ПО ПРЕДМЕТУ\n";
                    out << "Название: " << s->getName() << "\n";
                    out << "ID предмета: " << s->getId() << "\n";
        
                    if (s->getOwner())
                        out << "Преподаватель: " << s->getOwner()->getName() << "\n";
        
                    out << "----------------------------------------\n";
        
                    // Студенты
                    out << "\nСтуденты (" << s->getStudentsList().size() << "):\n";
                    for (auto* st : s->getStudentsList()) {
                        if (st)
                            out << " - " << st->getName()
                                << " (группа: " << st->getGroup() << ")\n";
                    }
        
                    // Задания
                    out << "\nЗадания:\n";
                    for (const auto& slot : s->getAssignmentsList()) {
                        if (!slot.work) continue;
        
                        out << " * " << slot.work->getTypeName()
                            << " \"" << slot.work->getTitle() << "\"";
        
                        if (!slot.reservedBy) {
                            out << " → свободно\n";
                        } else {
                            out << " → студент: " << slot.reservedBy->getName();
        
                            if (!slot.submitted && !slot.approved)
                                out << " | статус: записан\n";
                            else if (slot.submitted && !slot.approved)
                                out << " | статус: сдано, ждёт проверки\n";
                            else if (slot.approved)
                                out << " | оценка: " << slot.grade << "\n";
                        }
                    }
        
                    out << "\n--- конец отчёта ---\n";
                    out.close();
        
                    cout << "отчёт сохранён в файл: " << filename << "\n";
                    return;
                }
            }
            cout << "предмет не найден\n";
        }
        
        ~UniversitySystem() {
            // удаляем студентов
            for (auto* s : students)
                delete s;
        
            // удаляем преподавателей
            for (auto* t : teachers)
                delete t;
        
            // удаляем предметы 
            for (auto* sub : subjects)
                delete sub;
        }
        
    };
    
    void printMenu() {
        cout << "\n=== меню ===\n";
        cout << "1 - добавить преподавателя\n";
        cout << "2 - добавить студента\n";
        cout << "3 - создать предмет\n";
        cout << "4 - записать студента на предмет\n";
        cout << "5 - добавить задание на предмет (доклад/лаба)\n";
        cout << "6 - студент записывается на задание\n";
        cout << "7 - студент отмечает, что сдал\n";
        cout << "8 - преподаватель утверждает сдачу и ставит оценку\n";
        cout << "9 - преподаватель отклоняет сдачу\n";
        cout << "10 - студент спрыгивает с задания\n";
        cout << "11 - список предметов\n";
        cout << "12 - подробности по предмету (журнал)\n";
        cout << "13 - выгрузка отчёта по предмету\n";
        cout << "14 - список студентов\n";
        cout << "15 - список преподавателей\n";
        cout << "16 - преподаватели по предметам\n";
        cout << "17 - активность студента\n";
        cout << "0 - выход\n";
        cout << "выберите пункт: ";
    }
    
    int main() {
        setlocale(LC_ALL, "ru_RU.utf8");
        UniversitySystem sys;
    
        int choice = -1;
        while (true) {
            printMenu();
            cin >> choice;
    
            if (!cin) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "неверный ввод\n";
                continue;
            }
    
            if (choice == 0) {
                cout << "выход из программы\n";
                break;
            }
    
            switch (choice) {
            case 1:
                sys.addTeacher();
                break;
            case 2:
                sys.addStudent();
                break;
            case 3:
                sys.addSubject();
                break;
            case 4:
                sys.enrollStudentToSubject();
                break;
            case 5:
                sys.addWorkToSubject();
                break;
            case 6:
                sys.reserveWorkOnSubject();
                break;
            case 7:
                sys.studentSubmitWork();
                break;
            case 8:
                sys.approveWorkOnSubject();
                break;
            case 9:
                sys.rejectWorkOnSubject();
                break;
            case 10:
                sys.dropWorkOnSubject();
                break;
            case 11:
                sys.listSubjects();
                break;
            case 12:
                sys.showSubjectDetails();
                break;
            case 13:
                sys.exportSubjectReport();
                break;
            case 14:
                sys.listStudents();
                break;
            case 15:
                sys.listTeachers();
                break;
            case 16:
                sys.listTeachersBySubjects();
                break;
            case 17:
                sys.showStudentActivity();
                break;
            default:
                cout << "нет такого пункта\n";
                break;
            }
        }
    
        return 0;
    }
