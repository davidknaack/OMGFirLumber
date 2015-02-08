create table owners(
  owner_id INTEGER PRIMARY KEY,
  name varchar(50)
);

create table classes(
  class_id INTEGER PRIMARY KEY,
  name varchar(50),
  comment varchar(500)
);

create table cars(
  car_id INTEGER PRIMARY KEY,
  owner_id INTEGER NOT NULL REFERENCES owners(owner_id) ON DELETE CASCADE,
  class_id INTEGER NOT NULL REFERENCES classes(class_id) ON DELETE CASCADE,
  name varchar(50)
);

create table events(
  event_id INTEGER PRIMARY KEY,
  name varchar(50),
  date INTEGER,
  comment varchar(500)
);

create table races(
  race_id INTEGER PRIMARY KEY,
  event_id INTEGER NOT NULL REFERENCES events(event_id) ON DELETE CASCADE,
  complete INTEGER default 0,
  laneA_carID INTEGER,
  laneB_carID INTEGER,
  laneA_timeMs INTEGER,
  laneB_timeMs INTEGER,
  laneB_trapMs INTEGER
);
