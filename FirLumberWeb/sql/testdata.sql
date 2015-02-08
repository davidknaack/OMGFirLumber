-- Assumes a fresh database, FK's are hardcoded
insert into events (name, date, comment) values ("event 1", 1423433354, "first event");
insert into events (name, date, comment) values ("event 1", 1423443354, "second event");
insert into owners (name) values ("bob");
insert into owners (name) values ("sam");
insert into classes (name) values ("squares");
insert into classes (name) values ("cheaters");
insert into cars (owner_id, class_id, name) values (1, 1, "bob square1");
insert into cars (owner_id, class_id, name) values (1, 1, "bob square2");
insert into cars (owner_id, class_id, name) values (1, 2, "bob cheater1");
insert into cars (owner_id, class_id, name) values (2, 1, "sam square1");
insert into cars (owner_id, class_id, name) values (2, 2, "sam cheater1");
