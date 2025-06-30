begin;

delete from version;

drop rule version_no_insert on version;

alter table version add column date timestamp with time zone default now();
alter table version alter column date set not null;

insert into version (version) values (0);
insert into version (version) values (1);
insert into version (version) values (2);
insert into version (version) values (3);
insert into version (version) values (4);
insert into version (version) values (5);
insert into version (version) values (6);

commit;

