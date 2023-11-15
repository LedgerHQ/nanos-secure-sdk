from typing import List, Tuple

NB_MIN_PACKED_PIXELS = 3
NB_MAX_PACKED_PIXELS = 6
class Rle4bpp():

    @staticmethod
    def image_to_pixels(img, bpp: int) -> List[Tuple]:
        """
        Rotate and pack bitmap data of the character.
        """
        width, height = img.size

        color_indexes = []
        nb_colors = pow(2, bpp)
        base_threshold = int(256 / nb_colors)
        half_threshold = int(base_threshold / 2)

        # col first
        for col in reversed(range(width)):
            for row in range(height):
                # Return an index in the indexed colors list
                # top to bottom
                # Perform implicit rotation here (0,0) is left top in NBGL,
                # and generally left bottom for various canvas
                color_index = img.getpixel((col, row))
                color_index = int((color_index + half_threshold) / base_threshold)

                if color_index >= nb_colors:
                    color_index = nb_colors - 1

                color_indexes.append(color_index)

        return color_indexes


    @staticmethod
    def pixels_to_occurrences(pixels: List[int]) -> List[Tuple]:
        occurrences = []
        for pixel in pixels:
            if len(occurrences) == 0:
                occurrences.append((pixel, 1))
            else:
                color, cnt = occurrences[-1]
                if pixel == color:
                    occurrences[-1] = (pixel, cnt+1)
                else:
                    occurrences.append((pixel, 1))
        return occurrences

    # Fetch next single pixels that can be packed
    @classmethod
    def fetch_next_single_pixels(cls, occurrences: List[Tuple[int, int]]) -> List[int]:
        result = []
        for occurrence in occurrences:
            color, cnt = occurrence
            if cnt >= 2:
                break
            else:
                result.append(color)

        # Ensure pixels can be packed by groups
        nb_pixels = len(result)
        if (nb_pixels % NB_MAX_PACKED_PIXELS < NB_MIN_PACKED_PIXELS):
            return result[0:(nb_pixels - nb_pixels%NB_MIN_PACKED_PIXELS)]
        return result

    # Generate bytes from a list of single pixels
    def generate_packed_single_pixels_bytes(packed_occurences: List[int]) -> bytes:
        assert len(packed_occurences) >= 3
        assert len(packed_occurences) <= 6
        header = (0b10 << 2) | (len(packed_occurences) - 3)
        nibbles = [header]
        for occurrence in packed_occurences:
            nibbles.append(occurrence)

        result = []
        for i, nibble in enumerate(nibbles):
            if (i % 2) == 0:
                result.append(nibble << 4)
            else:
                result[-1] += nibble
        return bytes(result)

    @classmethod
    def handle_packed_pixels(cls, packed_occurences: List[int]) -> bytes:
        assert len(packed_occurences) >= 3
        result = bytes()
        for i in range(0, len(packed_occurences), NB_MAX_PACKED_PIXELS):
            result += cls.generate_packed_single_pixels_bytes(packed_occurences[i:i+NB_MAX_PACKED_PIXELS])
        return result

    @staticmethod
    def handle_white_occurrence(occurrence: Tuple[int, int]) -> bytes:
        _, cnt = occurrence
        unit_cnt_max = 64
        result = []

        for i in range(0, cnt, unit_cnt_max):
            diff_cnt = cnt - i
            if diff_cnt > unit_cnt_max:
                i_cnt = unit_cnt_max
            else:
                i_cnt = diff_cnt

            result.append((0b11 << 6) | (i_cnt-1))
        return bytes(result)

    @staticmethod
    def handle_non_white_occurrence(occurrence: Tuple[int, int]) -> bytes:
        color, cnt = occurrence
        unit_cnt_max = 8
        result = []

        for i in range(0, cnt, unit_cnt_max):
            diff_cnt = cnt - i
            if diff_cnt > unit_cnt_max:
                i_cnt = unit_cnt_max
            else:
                i_cnt = diff_cnt

            result.append((0 << 7) | (i_cnt-1) << 4 | color)

        return bytes(result)

    @classmethod
    def occurrences_to_rle(cls, occurrences: Tuple[int, int], bpp: int) -> bytes:
        result = bytes()
        WHITE_COLOR = pow(2, bpp) - 1
        i = 0
        while i < len(occurrences):
            # Check if next occurrences are packable in single occurrences
            single_pixels = cls.fetch_next_single_pixels(occurrences[i:])
            if len(single_pixels) > 0:
                # Pack single occurrences
                result += cls.handle_packed_pixels(single_pixels)
                i += len(single_pixels)
            else:
                # Encode next occurrence
                occurrence = occurrences[i]
                color, _ = occurrence
                if color == WHITE_COLOR:
                    result += cls.handle_white_occurrence(occurrence)
                else:
                    result += cls.handle_non_white_occurrence(occurrence)
                i += 1
        return result

    @classmethod
    def rle_4bpp(cls, img) -> Tuple[int, bytes]:
        bpp = 4
        pixels = cls.image_to_pixels(img, bpp)
        occurrences = cls.pixels_to_occurrences(pixels)
        return 1, cls.occurrences_to_rle(occurrences, bpp)

# -----------------------------------------------------------------------------
class Rle1bpp():
    """
    Class handling custom RLE encoding, optimized for 1PP.
    The generated bytes will contain alternances counts ZZZZOOOO with
    - ZZZZ: number of consecutives 0, from 0 to 15
    - OOOO: number of consecutives 1, from 0 to 15
    """
    # -------------------------------------------------------------------------
    @staticmethod
    def image_to_pixels(img, reverse):
        """
        Rotate and pack bitmap data of the character.
        return an array of pixels values.
        """
        width, height = img.size

        pixels = []
        # Intensity level value to be considered a white pixel
        white_threshold = 128
        if reverse:
            white_pixel = 0
            black_pixel = 1
        else:
            white_pixel = 1
            black_pixel = 0

        # col first
        for col in reversed(range(width)):
            for row in range(height):
                # Return an index in the indexed colors list (0 or 1, here)
                # top to bottom
                # Perform implicit rotation here (0,0) is left top in NBGL,
                # and generally left bottom for various canvas
                if img.getpixel((col, row)) >= white_threshold:
                    pixels.append(white_pixel)
                else:
                    pixels.append(black_pixel)

        return pixels

    # -------------------------------------------------------------------------
    @staticmethod
    def encode_pass1(data):
        """
        Encode array of values using 'normal' RLE.
        Return an array of tuples containing (repeat, val)
        """
        output = []
        previous_value = -1
        count = 0
        for value in data:
            # Same value than before?
            if value == previous_value:
                count += 1
            else:
                # Store previous result
                if count:
                    pair = (count, previous_value)
                    output.append(pair)
                # Start a new count
                previous_value = value
                count = 1

        # Store previous result
        if count:
            pair = (count, previous_value)
            output.append(pair)

        return output

    # -------------------------------------------------------------------------
    @staticmethod
    def encode_pass2(pairs):
        """
        Encode alternance counts between pixels (nb of 0, then nb of 1, etc)
        The generated packed values will contain ZZZZOOOO ZZZZOOOO with
        - ZZZZ: number of consecutives 0, from 0 to 15
        - OOOO: number of consecutives 1, from 0 to 15
        """
        max_count = 15
        # First step: store alternances, and split if count > 15
        next_pixel = 0
        alternances = []
        for repeat, value in pairs:
            # Store a count of 0 pixel if next pixel is not the one expected
            if value != next_pixel:
                alternances.append(0)
                next_pixel ^= 1

            while repeat > max_count:
                # Store 15 pixels of value next_pixel
                alternances.append(max_count)
                repeat -= max_count
                # Store 0 pixel of alternate value
                alternances.append(0)

            if repeat:
                alternances.append(repeat)
                next_pixel ^= 1

        # Now read all those values and store them into quartets
        output = bytes()
        index = 0

        while index < len(alternances):
            zeros = alternances[index]
            index += 1
            if index < len(alternances):
                ones = alternances[index]
                index += 1
            else:
                ones = 0

            byte = (zeros << 4) | ones
            output += bytes([byte])

        return output

    # -------------------------------------------------------------------------
    @staticmethod
    def remove_duplicates(pairs):
        """
        Check if there are some duplicated pairs (same values) and merge them.
        """
        index = len(pairs) - 1
        while index >= 1:
            repeat1, value1 = pairs[index-1]
            repeat2, value2 = pairs[index]
            # Do we have identical consecutives values?
            if value1 == value2:
                repeat1 += repeat2
                # Merge them and remove last entry
                pairs[index-1] = (repeat1, value1)
                pairs.pop(index)
            index -= 1

        return pairs

    # -------------------------------------------------------------------------
    @classmethod
    def decode_pass2(cls, data):
        """
        Decode packed bytes into array of tuples containing (repeat, value).
        The provided packed values will contain ZZZZOOOO ZZZZOOOO with
        - ZZZZ: number of consecutives 0, from 0 to 15
        - OOOO: number of consecutives 1, from 0 to 15
        """
        pairs = []
        for byte in data:
            ones = byte & 0x0F
            byte >>= 4
            zeros = byte & 0x0F
            if zeros:
                pairs.append((zeros, 0))
            if ones:
                pairs.append((ones, 1))

        # There was a limitation on repeat count => remove duplicates
        pairs = cls.remove_duplicates(pairs)

        return pairs

    # -------------------------------------------------------------------------
    @classmethod
    def rle_1bpp(cls, img, reverse) -> Tuple[int, bytes]:
        """
        Input: image to compress
        - convert the picture to an array of pixels
        - convert the array of pixels to tuples of (repeat, value)
        - encode using custom RLE
        Output: array of compressed bytes
        """
        pixels = cls.image_to_pixels(img, reverse)
        pairs = cls.encode_pass1(pixels)
        encoded_data = cls.encode_pass2(pairs)

        # Check if encoding/decoding is fine
        pairs2 = cls.decode_pass2(encoded_data)
        assert pairs == pairs2

        return 1, encoded_data
