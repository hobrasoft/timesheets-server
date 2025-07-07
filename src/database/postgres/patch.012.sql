begin;

insert into version (version) values (12);

alter table attendance.employees add column "user" integer;
alter table attendance.employees
    add constraint employees_user_fkey
    foreign key ("user") references users("user")
    on update cascade
    on delete set null;


alter table attendance.employees add column work_hours_mode interval default '8hours'::interval;
alter table attendance.employees add column rounding_interval interval default '10 minutes'::interval;
alter table attendance.employees add column saturdays_paid boolean default false;
alter table attendance.employees add column sundays_paid boolean default false;
alter table attendance.employees add column auto_breaks boolean default false;
alter table attendance.employees add column overtime_paid boolean default false;

update attendance.employees set work_hours_mode = '8 hours'::interval;
update attendance.employees set rounding_interval = '10 minutes'::interval;
update attendance.employees set saturdays_paid = false;
update attendance.employees set sundays_paid = false;
update attendance.employees set auto_breaks = false;
update attendance.employees set overtime_paid = false;

alter table attendance.employees alter column work_hours_mode set not null;
alter table attendance.employees alter column rounding_interval set not null;
alter table attendance.employees alter column saturdays_paid set not null;
alter table attendance.employees alter column sundays_paid set not null;
alter table attendance.employees alter column auto_breaks set not null;
alter table attendance.employees alter column overtime_paid set not null;

commit;
