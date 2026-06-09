with open("./data/item-lot.txt", "r") as my_f:
    my = my_f.readlines()

with open("./data/OtherLots.txt", "r") as his_f:
    his = his_f.readlines()

skip_ngp = True

class Info:
    index: str
    info: str
    desc: str
    ng: bool

    def __init__(self, index: str, desc: str, info: str = "Area", ng: bool = True):
        self.index = index
        self.desc = desc
        self.info = info
        self.ng = ng

    def __repr__(self) -> str:
        return f"ID: {self.index}, {self.info}, {self.desc}, ({"NG" if self.ng else "NG+"})";

HisInfos: list[Info] = []

start = False
for i in his:
    i = i.strip();
    if (i == ""): continue;
    split = i.split(";")
    if (len(split) == 1):
        title = split[0];
        if (title == "#UNMISSABLE"): start = True;
        elif (title == "#MISSABLE"): break;
    elif start:
        index, info_desc = split
        ds = info_desc.index("[");
        de = info_desc.index("]");
        info = info_desc[ds+1:de]
        desc = info_desc[de+1:]
        type_info = [j.strip() for j in info.split(" - ")]
        ng = "NG+" not in info and "NG+" not in desc
        if (len(type_info) == 1):
            if (type_info[0]) == "Event":
                info = "Event"
            else:
                info = "Area"
        elif "Event" in type_info:
            desc = " - ".join(k for k in type_info[1:]) + f" ({desc.strip()})"
            info = "Event"
        elif "NPC" in type_info:
            desc = " - ".join(k for k in type_info[1:]) + f" ({desc.strip()})"
            info = "NPC"
        elif "Boss" in type_info:
            desc = " - ".join(k for k in type_info[1:]) + f" ({desc.strip()})"
            info = "Boss"
        elif "Chest" in type_info:
            desc = " - ".join(k for k in type_info) + f" ({desc.strip()})"
            info = "Chest"
        else:
            info = "Area"

        HisInfos.append(Info(index, desc.strip(), info, ng))

his_infos_dict: dict[str, Info] = {};
for i in HisInfos:
    his_infos_dict[i.index] = i;


class Item:
    item: str
    keys: list[str]
    desc: str = ""
    typ: str = ""

    def __init__(self, item: str, keys: list[str]):
        self.item = item.strip()
        self.keys = keys

    def __repr__(self):
        if his_infos_dict.get(self.item):
            his_info = his_infos_dict[self.item]
        else:
            raise ValueError(f"{self.item} not found")
        keys = ", ".join(i.replace("#", "") for i in self.keys)
        details = f" = {{\n\t\tKeys = [{keys}],\n\t\tType = {his_info.info}({"NG" if his_info.ng else "NG+"}),\n\t\tDesc = {his_info.desc.strip()}\n\t}}"
        return f"{self.item}{details}"

class MyInfo:
    gate_name: str
    items: list[Item]

    def __init__(self, name: str, items: list[Item]):
        self.gate_name = name;
        self.items = items;

    def __repr__(self):
        if len(self.items) == 0:
            items = ""
        else:
            items = f"{{\n\t{',\n\t'.join(str(i) for i in self.items)}\n}}"
        return f"{self.gate_name} = {items}"

my_items: dict[str, MyInfo] = {};
count = 0;
cur_gate: str = ""
cur_key: list[str] = []
my_infos: dict[str, MyInfo] = {}
all_items: dict[str, bool] = {}
for i in my:
    count += 1;
    if ("GuranteedDrop" in i): break; # after this is parsed later
    i = i.strip()
    if (i == ""): continue;
    split = i.split(",")
    if (len(split) != 1): # is item id 
        assert(i.endswith(","))
        keys = cur_key.copy()
        items = i.split(",")
        for item in items:
            if (item == ""):
                continue
            if all_items.get(item) == None:
                all_items[item] = True;
            else:
                assert(item == "60013000" or item == "60004000"); #Allow duplicates only for soul of nadalia fragment and giant soul
            my_infos[cur_gate].items.append(Item(item, keys))
    elif (i.startswith("##")): # is a key
        hash_count = i.count("#")
        prev_count = 0
        if (len(cur_key) > 0):
            prev_count = cur_key[-1].count("#")
        if (hash_count > prev_count):
            cur_key.append(i);
        elif (hash_count == prev_count):
            cur_key[-1] = i
        else:
            cur_key[-2] = cur_key[-1]
            cur_key.pop()
    else: # is a gate
        cur_gate = i;
        if my_infos.get(i) == None:
            my_infos[i] = MyInfo(i, [])
        cur_key.clear()

# this is parsing the guranteed drops
cur_key: list[str] = []
for i in my[count:]:
    i = i.strip()
    if (i == ""): continue;
    split = i.split(",")
    if (len(split) != 1): # is item id 
        item, *desc = i.split(",")
        if all_items.get(item) == None:
            all_items[item] = True;
        else:
            raise ValueError(f"{item} already added")
        it = Item(item, cur_key.copy())
        it.desc = ", ".join(j.strip() for j in desc)
        it.typ = "GuranteedDrop"
        my_infos[cur_gate].items.append(it)
    elif (i.startswith("##")): # is a key
        hash_count = i.count("#")
        prev_count = 0
        if (len(cur_key) > 0):
            prev_count = cur_key[-1].count("#")
        if (hash_count > prev_count):
            cur_key.append(i);
        elif (hash_count == prev_count):
            cur_key[-1] = i
        else:
            cur_key[-2] = cur_key[-1]
            cur_key.pop()
    else: # is a gate
        cur_gate = i;
        if my_infos.get(i) == None:
            my_infos[i] = MyInfo(i, [])
        cur_key.clear()

with open("./data/item-lot-formatted.txt", "w") as f:
    for v in my_infos.values():
        f.write(f"{v.gate_name}[{len(v.items)}] = {{\n")
        for i in v.items:
            f.write(f"\t{i.item} = {{\n")
            f.write(f"\t\tKeys[{len(i.keys)}] = {{{", ".join(j.replace("#", "") for j in i.keys)}}},\n")
            if (i.typ == ""):
                if his_infos_dict.get(i.item):
                    his_info = his_infos_dict[i.item]
                else:
                    raise ValueError(f"{i.item} not found")
                f.write(f"\t\tType = {his_info.info}\n")
            else:
                f.write(f"\t\tType = {i.typ}\n")
            if i.desc == "":
                if his_infos_dict.get(i.item):
                    his_info = his_infos_dict[i.item]
                else:
                    raise ValueError(f"{i.item} not found")
                f.write(f"\t\tDesc = {his_info.desc}\n")
            else:
                f.write(f"\t\tDesc = {i.desc}\n")
            f.write("\t},\n")
        f.write("}\n\n")

with open("./item_info.c", "w") as f:
    f.write("void generate_item_infos() {\n")
    f.write("\tItemInfoGroup iig;\n")
    f.write(f"\tKeys keys;\n")
    for v in my_infos.values():
        f.write("\tiig = (ItemInfoGroup){0};\n")
        f.write(f"\tda_init(&iig, {len(v.items)});\n")
        for i in v.items:
            f.write(f"\tkeys = (Keys){{0}};\n")
            f.write(f"\tda_init(&keys, {len(i.keys)});\n")
            for k in i.keys:
                f.write(f"\tda_append(&keys, {k.replace("#", "")});\n")
            if i.typ == "":
                if his_infos_dict.get(i.item):
                    his_info = his_infos_dict[i.item]
                else:
                    raise ValueError(f"{i.item} not found")
                i.typ = his_info.info;
            if i.desc == "":
                if his_infos_dict.get(i.item):
                    his_info = his_infos_dict[i.item]
                else:
                    raise ValueError(f"{i.item} not found")
                i.desc = his_info.desc;

            f.write(f"\tda_append(&iig, ((ItemInfo){{.keys = keys, .type = Key{i.typ}, .desc = \"{i.desc}\"}}));\n");

        f.write(f"\t*ht_put(&gate_item_infos, {v.gate_name.replace("#", "")}) = iig;\n\n");
    f.write("}\n")
