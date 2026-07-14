#define VARINT_LONG
#define int s32
#define Varint s64
#define short s16
#define byte u8

struct Header {
    int unk00;
#ifdef VARINT_LONG
    int unk04;
#endif
    Varint spriteSetsOffset;
    Varint offset2;
    Varint nodesOffset;
    Varint animatorsOffset;
    Varint rootsOffset;
    Varint texturesOffset;
    Varint stringsOffset;
    Varint textsOffset;
    short spriteSetCount;
    short count2;
    short nodeCount;
    short animatorCount;
    short rootCount;
    short textureCount;
    short stringCount;
    short textCount;
    byte bigEndian;
    byte unk59;
    byte unk5A;
    byte unk5B;
    int unk5C;
    int unk60;
    int unk64;
    int unk68;
    int unk6C;
    int unk70;
    int unk74;
    int unk78;
    int unk7C;
    int unk80;
    int unk84;
    int unk88;
    int unk8C;
    int unk90;
    int unk94;
    int unk98;
    int unk9C;
    int unkA0;
#ifdef VARINT_LONG
    int unkA4;
#endif
};

struct V2f {
 float x,y;
};

struct Texture {
    Varint name_offset;
    short id;
    short unk0A; //Assert(unk0A == 0);
    short unk0C; //Assert(unk0C == 0);
    short width; // These don't seem to actually do anything
    short height;
    short unk12; //Assert(unk12 == 0);
#ifdef VARINT_LONG
    int unk14;
#endif
    //char name[]@name_offset;
};

struct TexRegion {
    V2f v1;
    V2f v2;
};

struct Sprite {
    V2f position;
    V2f scale;
    V2f skew;
    int unk18;// Assert(unk18 == -1);
#ifdef VARINT_LONG
    int pad1C; //Assert(pad1C == 0);
#endif
    Varint textureOffset;
    short textureID;
    short unk2A; //Assert(unk2A == 0);
#ifdef VARINT_LONG
    int pad2C; //Assert(pad2C == 0);
#endif
    Varint texRegionOffset;
    int unk38; //Assert(unk38 == 0);
    int unk3C; //Assert(unk3C == 0);
    //Texture texture@textureOffset;
    TexRegion texRegion@texRegionOffset;
};

struct SpriteSet {
    short id;
    short spriteCount;
#ifdef VARINT_LONG
    int pad04; //Assert(pad04 == 0);
#endif
    Varint spritesOffset;
    int unk10; //Assert(unk10 == 0);
    int unk14; //Assert(unk14 == 0);
    Sprite sprites[spriteCount]@spritesOffset;
};

#define ubyte u8

struct Frame {
    V2f translation;
    V2f scale;
    V2f skew;
    ubyte color[4]; // RGBA
    int unk1C; // Usually 0, possibly another color?
    int unk20;
    int unk24;
    int unk28; //Assert(unk28 == 0);
    int unk2C; //Assert(unk2C == 0);
};

struct Animator {
    short elementID;
    short frameCount;
#ifdef VARINT_LONG
        int pad04; //Assert(pad04 == 0);
#endif
    Varint framesOffset;
    short unk10;
    short unk12;
    short unk14;
    short unk16;
    int unk18;
    int unk1C;
    int unk20; //Assert(unk20 == 0);
    int unk24; //Assert(unk24 == 0);
    Frame frames[frameCount]@framesOffset;
};


struct NodeItemA {
    int unk00;
    short unk04;
    short unk06; //Assert(unk06 == 0);
    int unk08; //Assert(unk08 == 0);
    int unk0C; //Assert(unk0C == 0);
};

struct NodeListA {
    Varint nodeItemsAOffset;
    short nodeItemACount;
    short unk0A; //Assert(unk0A == 0);
    int unk0C; //Assert(unk0C == 0);
    int unk10; //Assert(unk10 == 0);
#ifdef VARINT_LONG
    int unk14;
#endif
    if (nodeItemsAOffset != 0) {
        NodeItemA node_items[nodeItemACount]@nodeItemsAOffset;
    }
};

struct NodeItemB {
    int unk00;
    short unk04;
    short unk06;
    int unk08;
    int unk0C; //Assert(unk0C == 0);
    int unk10; //Assert(unk10 == 0);
    int unk14; //Assert(unk14 == 0);
    int unk18; //Assert(unk18 == 0);
    int unk1C; //Assert(unk1C == 0);
};


struct NodeListB {
    Varint nodeItemsBOffset;
    short nodeItemBCount;
    short unk0A; //Assert(unk0A == 0);
    int unk0C; //Assert(unk0C == 0);
    int unk10; //Assert(unk10 == 0);
#ifdef VARINT_LONG
        int unk14;
#endif
    if (nodeItemsBOffset != 0) {
        NodeItemB node_items[nodeItemBCount]@nodeItemsBOffset;
    }
};

struct Node {
    short id;
    short animatorCount;
#ifdef VARINT_LONG
        int unk04;
#endif
    Varint animatorsOffset;
    int unk10;
    short unk14; //Assert(unk14 == 1);
    short unk16; //Assert(unk16 == 0);
    Varint nodeListAOffset;
    Varint nodeListBOffset;
    
    // Dubious
    Varint unk28; //Assert(unk28.val == 0);
    Varint unk2C; //Assert(unk2C.val == 0);
    int unk30; //Assert(unk30 == 0);
    int unk34; //Assert(unk34 == 0);
    int unk38; //Assert(unk38 == 0);
    int unk3C; //Assert(unk3C == 0);
    
    //Animator animators[animatorCount]@animatorsOffset;
    if (nodeListAOffset != 0) {
        NodeListA nodelista@nodeListAOffset;
    }
    if (nodeListBOffset != 0) {
        NodeListB nodelistb@nodeListBOffset;
    }
};

struct Root {
    int id;
    int nodeID;
    Varint nodeOffset;
    V2f position;
    short unk18;
    short unk1A; // byte byte?
    int unk1C; // In SotfS these two are 0
    int unk20; // In DX9 this looks like SotfS's unk24
#ifdef VARINT_LONG
        int unk24;
#endif
    //Node node@nodeOffset;
};

struct String {
    char name[];
};

struct Text {
    short id;
    short unk02; //Assert(unk02 == 0);
    short font;
    short unk06;
    short size;
    short unk0A; //Assert(unk0A == 0);
    short unk0C;
    short unk0E; //Assert(unk0E == 0);
    short unk10;
    ubyte color[4]; // RGBA
    byte unk16;
    byte unk17;
    // Dubious
    Varint unk18; //Assert(unk18.val == 0);
    int unk20; //Assert(unk20 == 0);
    int unk24; //Assert(unk24 == 0);
    int unk28; //Assert(unk28 == 0);
    int unk2C; //Assert(unk2C == 0);
    int unk30; //Assert(unk30 == 0);
#ifdef VARINT_LONG
    int unk34;
#endif
};

struct FLO {
    Header header;
    SpriteSet spriteSets[header.spriteSetCount]@header.spriteSetsOffset;
    Node nodes[header.nodeCount]@header.nodesOffset;
    Root roots[header.rootCount]@header.rootsOffset;
    Animator animators[header.animatorCount]@header.animatorsOffset;
    Texture textures[header.textureCount]@header.texturesOffset;
    String names[header.stringCount]@header.stringsOffset;
    Text texts[header.textCount]@header.textsOffset;
};

FLO flo@0x00;