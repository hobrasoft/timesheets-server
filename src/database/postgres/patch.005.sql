begin;

create schema attendance;

set search_path to attendance;


create table departments (
    department      serial primary key,
    abbr            text not null,
    description     text not null
);

create table doors (
    door            serial primary key,
    description     text not null
);

create table rfids (
    rfid            serial primary key,
    rfid_id         text not null unique,
    valid           boolean,
    note            text
);

create table employees (
    employee        serial primary key,
    firstname       text not null,
    surname         text not null,
    active          boolean
);

create table event_types (
    event_type  text primary key,
    description text not null,
    -- dále česky
    passage             boolean,    -- ignored, for door opening
    end_state           boolean,    -- ends the previous state
    arrival             boolean,    -- arrival at work
    vacation            boolean,    -- start of vacation
    sick_leave          boolean,    -- start of sick leave
    compensatory_leave  boolean,    -- start of compensatory leave
    business_trip       boolean,    -- start of business trip
    break_time          boolean,    -- start of break
    unpaid_leave        boolean,    -- start of unpaid leave
    sick_care           boolean,    -- start of OČR (childcare leave)
    paid_obstacle       boolean,    -- start of paid obstacle
    doctor              boolean     -- start of doctor visit
);

create table events (
    event       serial primary key,
    date        timestamp with time zone,
    event_type  text not null references event_types(event_type) on update cascade on delete no action,
    employee    integer not null references employees(employee) on update cascade on delete cascade,
    valid       boolean,
    user_edited integer references public.users("user") on update cascade on delete no action
);

create table department_has_manager (
    department      integer not null references departments(department) on update cascade on delete cascade,
   "user"           integer not null references public.users("user") on update cascade on delete no action
);

create table department_has_member (
    department      integer not null references departments(department) on update cascade on delete cascade,
    employee        integer not null references employees(employee) on update cascade on delete cascade,
    primary key (department, employee)
);

create table employee_can_open_door (
    employee        integer not null references employees(employee) on update cascade on delete cascade,
    door            integer not null references doors(door) on update cascade on delete cascade,
    primary key (employee, door)
);

create table employee_has_rfid (
    rfid            integer not null references rfids(rfid) on update cascade on delete cascade,
    employee        integer not null references employees(employee) on update cascade on delete cascade,
    primary key (rfid),
    unique (rfid, employee)
);


commit;

