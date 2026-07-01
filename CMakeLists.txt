cmake_minimum_required(VERSION 3.30)
project(Entregable1)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)



file(GLOB SOURCES "src/*.cpp")

add_executable(Entregable1 ${SOURCES}
        include/PlanFactory.h
        include/PlanSession.h
        include/LikeObserver.h
        include/RankingStrategy.h
        include/SearchMatchStrategy.h)