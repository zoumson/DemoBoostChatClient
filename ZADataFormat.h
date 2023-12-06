#pragma once

namespace za
{
	namespace net
	{

		namespace cnt
		{
			constexpr size_t ZA_SIZE = 8;
			constexpr size_t ZA_HEADER_SIZE = 3;
			constexpr size_t ZA_BODY_SIZE = 5;

			enum ZA_PACKET
			{
				HEADER_1 = 0,
				HEADER_2,
				HEADER_3,

				BODY_1,
				BODY_2,
				BODY_3,
				BODY_4,
				BODY_5,
			};

			enum ZA_HEADER_1
			{
				HEADER_1_1 = 1,
				HEADER_1_2,
			};
			enum ZA_HEADER_2
			{
				HEADER_2_1 = 1,
				HEADER_2_2,
			};
			enum ZA_HEADER_3
			{
				HEADER_3_1 = 1,
				HEADER_3_2,
			};

		}

	}
}
