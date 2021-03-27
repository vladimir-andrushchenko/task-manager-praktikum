#include <map>
#include <string>
#include <tuple>
#include <vector>
#include <iostream>

using namespace std;

// Перечислимый тип для статуса задачи
enum class TaskStatus {
      NEW,          // новая
      IN_PROGRESS,  // в разработке
      TESTING,      // на тестировании
      DONE          // завершена
};


// Объявляем тип-синоним для map<TaskStatus, int>,
// позволяющего хранить количество задач каждого статуса
using TasksInfo = map<TaskStatus, int>;

class TeamTasks {
public:
      // Получить статистику по статусам задач конкретного разработчика
    const TasksInfo& GetPersonTasksInfo(const string& person) const {
        return persons_tasks_.at(person);
    }
      
      // Добавить новую задачу (в статусе NEW) для конкретного разработчитка
    void AddNewTask(const string& person) {
        persons_tasks_[person][TaskStatus::NEW]++;
    }
      
    // Обновить статусы по данному количеству задач конкретного разработчика,
    // подробности см. ниже
    tuple<TasksInfo, TasksInfo> PerformPersonTasks(const string& person, int task_count) {
        if (persons_tasks_.count(person) == 0) {
            return {{},{}};
        }
        // 1. Рассмотреть все невыполненные задачи разработчика person.
        TasksInfo tasks_a_person_has = persons_tasks_[person];
        // 2. Упорядочить их по статусам: сначала все задачи в статусе NEW, затем все задачи в статусе IN_PROGRESS и, наконец, задачи в статусе TESTING.
        vector<int> ordered_untouched_tasks = {tasks_a_person_has[TaskStatus::NEW],
                                     tasks_a_person_has[TaskStatus::IN_PROGRESS],
                                     tasks_a_person_has[TaskStatus::TESTING],
        };
        // Обновление словаря одновременно с итерированием по нему может привести к непредсказуемым последствиям
        const vector<int> temp_ordered_untouched_tasks = ordered_untouched_tasks;
        // Сначала соберите информацию об обновлениях в отдельном временном словаре, а потом примените их к основному словарю.
        vector<int> updated_tasks(4);
        //Рассмотреть первые task_count задач и перевести каждую из них в следующий статус в соответствии с естественным порядком: NEW → IN_PROGRESS → TESTING → DONE.
        int counter = 0;
        for (const int tasks : temp_ordered_untouched_tasks) {
            if (tasks > 0) {
                int diff = tasks - task_count;
                if (diff >= 0) { // если разница больше нуля то значит счетчика заданий хватит только на перевод заданий из текущей категории
                    ordered_untouched_tasks[counter] = diff;
                    updated_tasks[counter + 1] += task_count; // и отправляю их в следующий
                    break; // task_count никак не трогаю потому что можно его только обнулить, а делать этого нет смысла потому что я уже выхожу из цикла
                } else if (diff < 0) { // если разница меньше нуля, значит счетчика заданий заватит и на селующие категории
                    task_count = abs(diff); // оставшийся счетчик
                    updated_tasks[counter + 1] = tasks;  // сохраню сколько заданий перейдут в следующий статус
                    ordered_untouched_tasks[counter] = 0;
                }
            }
            counter++;
        }
        
        // нужно обновить статус
        // сначала получаю статус в виде вектора
        vector<int> new_tasks_state;
        for (int i = 0; i < 3; i++) {
            new_tasks_state.push_back(updated_tasks[i] + ordered_untouched_tasks[i]);
        }
        new_tasks_state.push_back(updated_tasks[3] + tasks_a_person_has[TaskStatus::DONE]); // не забываю добавить новые выполненные задачи к уже существующим
        // и вставляю его
        for (int i = 0; i < 4; i++) {
            persons_tasks_[person][static_cast<TaskStatus>(i)] = new_tasks_state[i];
        }
        
        // подготавливаю выходные данные
        TasksInfo untouched_tasks_info;
        for (int i = 0; i < 3; i++) {
            if (ordered_untouched_tasks[i] == 0) {
                continue;
            }
            untouched_tasks_info[static_cast<TaskStatus>(i)] = ordered_untouched_tasks[i];
        }
        TasksInfo updated_tasks_info;
        for (int i = 0; i < 4; i++) {
            if (updated_tasks[i] == 0) {
                continue;
            }
            updated_tasks_info[static_cast<TaskStatus>(i)] = updated_tasks[i];
        }
        return {updated_tasks_info, untouched_tasks_info};
    }
private:
    map<string, TasksInfo> persons_tasks_;
};
