#ifndef REGISTRY_TEST_H_
#define REGISTRY_TEST_H_

#include "test.h"

class Person {
public:
    Person(const std::string& name, unsigned int num) : m_name(name), m_num(num) {}
    const std::string& name(void) const { return m_name; }
    unsigned int num(void) const { return m_num; }
private:
    std::string m_name;
    unsigned int m_num;
};

class PersonPred {
public:
    PersonPred(const std::string& name, unsigned int num) : name(name), n(num) {

    }
    bool operator()(const Person* p) {
        return p != nullptr && p->name() == name && p->num() == n;
    }
private:
    std::string name;
    unsigned int n;
};

class People : public Registry<Person> {
public:
    Person *regNew(const char* name, std::unique_ptr<Person> &&person) {
        return Registry<Person>::registerNew(name, std::move(person));
    }
    void clear() {
        Registry<Person>::unregisterAll();
    }
    Person* getPerson(const char* name) {
        return Registry<Person>::get(name);
    }
};

class PeopleWithPred : public RegistryWithPred<Person, PersonPred> {
public:
    Person *regNew(std::unique_ptr<Person> &&person) {
        return RegistryWithPred<Person, PersonPred>::registerNew(std::move(person));
    }
    void clear() {
        RegistryWithPred<Person, PersonPred>::unregisterAll();
    }
    Person* get(const std::string& name, unsigned int numb) {
        return RegistryWithPred<Person, PersonPred>::get(name, numb);
    }
};

/// Tests for usage of registry (Thread unsafe but its OK with gtest)
TEST(RegistryTest, RegisterAndUnregister) {
    People people;
    people.regNew("John", std::make_unique<Person>("John", 433212345));
    people.regNew("John", std::make_unique<Person>("John", 123456));

    EXPECT_EQ(1, people.size());
    auto *john = people.getPerson("John");
    ASSERT_NE(john, nullptr);
    unsigned int n = john->num();
    EXPECT_EQ(n, 123456);

    People people2;
    people2 = people;
    EXPECT_EQ(1, people2.size());
    EXPECT_EQ(1, people.size());

    people.clear();
    EXPECT_TRUE(people.empty());
    EXPECT_EQ(1, people2.size());
    people2.clear();
    EXPECT_TRUE(people2.empty());

    PeopleWithPred peopleWithPred;
    peopleWithPred.regNew(std::make_unique<Person>("McDonald", 123));
    peopleWithPred.regNew(std::make_unique<Person>("McDonald", 157));
    EXPECT_EQ(peopleWithPred.size(), 2);

    Person *p = peopleWithPred.get("McDonald", 157);
    EXPECT_EQ(p->name(), "McDonald");
    EXPECT_EQ(p->num(), 157);

    PeopleWithPred peopleWithPred2;
    peopleWithPred2 = peopleWithPred;
    EXPECT_EQ(peopleWithPred.size(), 2);
    EXPECT_EQ(peopleWithPred2.size(), 2);

    peopleWithPred.clear();
    EXPECT_TRUE(peopleWithPred.empty());
    peopleWithPred2.clear();
    EXPECT_TRUE(peopleWithPred2.empty());
}

#endif // REGISTRY_TEST_H_
