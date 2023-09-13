#define WRITE_WIDTH
#define WRITE_HEIGHT
#define GROUP_WIDTH (WRITE_WIDTH + 2)
#define GROUP_HEIGHT (WRITE_HEIGHT + 2)

Texture2D<uint64_t> tex_In;
RWTexture2D<uint64_t> tex_Out;

groupshared uint64 gs_Cache[GROUP_WIDTH][GROUP_HEIGHT];

struct Input {
  // TODO
};
cbuffer ConstantBuffer {
  // Must be powers of 2
  uint2 cb_Size;
}

inline uint GetCell(uint x, uint y, uint2 gtid) {
  uint64 v = gs_Cache[gtid.x + (x >> 4)][gtid.y + (y >> 4)];
  uint bitOff = (x & 15) + ((y & 15) << 4);
  return v & (1 << bitOff);
}

[BlockSize(GROUP_WIDTH, GROUP_HEIGHT)]
void main(Input IN) {
  // Load data
  uint2 uv = { (gtid.gid.x * WRITE_WIDTH + gtid.x - 1) & cb_Size.x,
    (gtid.gid.y * WRITE_HEIGHT + gtid.y - 1) & cb_Size.y };
  gs_Cache[IN.gtid.x][IN.gtid.y] = tex_In[uv];
  SharedMemoryBarrierWithGroupSync();
  // Filter out borders, which were just for data loading
  if (IN.gtid.x == 0 || gtid.y == 0 || gtid.x == GROUP_WIDTH - 1 || gtid.y == GROUP_HEIGHT - 1)
    return;
  // Each bit is a cell
  for (uint i = 0; i < 64; i++) {
    int y = i & 15;
    int x = (i << 4) & 15;
    // Iterate over neighbours
    uint c =
      GetCell(y - 1, x - 1, gtid.xy) + GetCell(y - 1, x, gtid.xy) + GetCell(y - 1, x + 1, gtid.xy)
      + GetCell(y - 1, x - 1, gtid.xy) /* Skip Middle */ + GetCell(y - 1, x + 1, gtid.xy)
      + GetCell(y + 1, x - 1, gtid.xy) + GetCell(y + 1, x, gtid.xy) + GetCell(y + 1, x + 1, gtid.xy);
    // Use number of adjacent active tiles (c) to get what the next state should be
    // TODO
  }
}
