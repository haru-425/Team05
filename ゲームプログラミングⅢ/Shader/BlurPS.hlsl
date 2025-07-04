/**
 * @file BlurPS.hlsl
 * @brief �K�E�X�ڂ����ƃu���[�����ʂ�K�p����s�N�Z���V�F�[�_
 * 
 * ���̃V�F�[�_�́A���̓e�N�X�`���ɑ΂��ăK�E�X�ڂ������s���A�u���[�����ʂ���������A
 * �g�[���}�b�s���O�iHDR��SDR�j��K�p���čŏI�I�ȐF���o�͂��܂��B
 * 
 * @see https://en.wikipedia.org/wiki/Gaussian_blur
 * @see https://en.wikipedia.org/wiki/Bloom_(shader_effect)
 */

#include "fullscreenQuad.hlsli"

/// �T���v���[�X�e�[�g�̃C���f�b�N�X��`
#define POINT 0         ///< �ŋߖT��ԃT���v���[
#define LINEAR 1        ///< ���`��ԃT���v���[
#define ANISOTROPIC 2   ///< �ٕ����t�B���^�����O�T���v���[

/// �T���v���[�X�e�[�g�z��B3��ނ̃T���v�����O���@���i�[�B
SamplerState sampler_states[3] : register(s0);

/// �e�N�X�`���z��B0:���摜, 1:�ڂ����Ώ�, 2,3:���g�p�܂��͊g���p�B
Texture2D texture_maps[4] : register(t10);

/**
 * @brief �s�N�Z���V�F�[�_�̃��C���֐�
 * 
 * ���͂��ꂽ�e�N�X�`�����W�Ɋ�Â��A�K�E�X�ڂ����ƃu���[�����ʂ�K�p�����F���o�͂��܂��B
 * 
 * @param[in] pin ���_�V�F�[�_����n�����\���́B�e�N�X�`�����W�Ȃǂ��܂ށB
 * @return float4 �ŏI�I�ȐF�iRGBA�j
 */
float4 main(VS_OUT pin) : SV_TARGET
{
    /// �e�N�X�`���̃~�b�v���x���i�ʏ�0�j�A���A�����A�~�b�v���x�������i�[����ϐ�
    uint mip_level = 0, width, height, number_of_levels;
    texture_maps[1].GetDimensions(mip_level, width, height, number_of_levels);

    /// ���摜�̐F���ٕ����T���v���[�Ŏ擾
    float4 color = texture_maps[0].Sample(sampler_states[ANISOTROPIC], pin.texcoord);

    /// ���摜�̃A���t�@�l��ێ�
    float alpha = color.a;

    /// �ڂ�����̐F���i�[����ϐ��i�����l0�j
    float3 blur_color = 0;

    /// �K�E�X�J�[�l���̍��v�l�i���K���p�j
    float gaussian_kernel_total = 0;

    // --- �K�E�X�ڂ����̃p�����[�^��` ---
    /**
     * @brief �K�E�X�J�[�l���̔��a
     * 
     * ��: 3�̏ꍇ�A�J�[�l���T�C�Y��7x7�i-3�`+3�j�ƂȂ�B
     * �l��傫������Ƃڂ����͈͂��L����B
     */
    const int gaussian_half_kernel_size = 10;

    /**
     * @brief �K�E�X���z�̕W���΍�
     * 
     * �l��傫������Ƃڂ������Ȃ��炩�ɁA�l������������ƃV���[�v�ɂȂ�B
     */
    const float gaussian_sigma = 1.0;

    /**
     * @brief �u���[�����ʂ̋��x
     * 
     * �ڂ����������ǂꂾ�����摜�ɉ��Z���邩�����肷��B
     */
    const float bloom_intensity = 2.0;

    // --- �K�E�X�ڂ������� ---
    /**
     * @details
     * �e�N�X�`�����W���ӂ̃s�N�Z�����K�E�X���z�Ɋ�Â��ăT���v�����O���A���d���ς��v�Z����B
     * ����ɂ��A�摜�S�̂��_�炩���ڂ���B
     * 
     * @see https://en.wikipedia.org/wiki/Gaussian_blur
     */
    [unroll]
    for (int x = -gaussian_half_kernel_size; x <= +gaussian_half_kernel_size; x += 1)
    {
        [unroll]
        for (int y = -gaussian_half_kernel_size; y <= +gaussian_half_kernel_size; y += 1)
        {
            /// �K�E�X�J�[�l���̏d�݂��v�Z
            float gaussian_kernel = exp(-(x * x + y * y) / (2.0 * gaussian_sigma * gaussian_sigma)) /
                  (2 * 3.14159265358979 * gaussian_sigma * gaussian_sigma);

            /// ���Ӄs�N�Z������`��ԃT���v���[�Ŏ擾���A�K�E�X�d�݂��|���ĉ��Z
            blur_color += texture_maps[1].Sample(
                sampler_states[LINEAR],
                pin.texcoord + float2(x * 1.0 / width, y * 1.0 / height)
            ).rgb * gaussian_kernel;

            /// �J�[�l�����v�l���X�V�i���K���p�j
            gaussian_kernel_total += gaussian_kernel;
        }
    }

    /// �ڂ����F���J�[�l�����v�l�Ő��K��
    blur_color /= gaussian_kernel_total;

    /// ���摜�Ƀu���[���i�ڂ����j���������Z
    color.rgb += blur_color * bloom_intensity;

#if 1
    // --- �g�[���}�b�s���O�����iHDR��SDR�j ---
    /**
     * @brief �g�[���}�b�s���O
     * 
     * HDR�摜��SDR�ɕϊ����A���邳�𒲐�����B
     * exposure�l��傫������ƑS�̓I�ɖ��邭�Ȃ�B
     * 
     * @see https://en.wikipedia.org/wiki/Tone_mapping
     */
    const float exposure = 1.2;
    color.rgb = 1 - exp(-color.rgb * exposure);
#endif

#if 0
    // --- �K���}�␳���� ---
    /**
     * @brief �K���}�␳
     * 
     * �f�B�X�v���C�\���p�ɐF�𒲐�����B�ʏ�2.2����ʓI�B
     * 
     * @see https://en.wikipedia.org/wiki/Gamma_correction
     */
    const float GAMMA = 2.2;
    color.rgb = pow(color.rgb, 1.0 / GAMMA);
#endif

    /// �ŏI�I�ȐF�iRGB�j�ƌ��摜�̃A���t�@�l��Ԃ�
    return float4(color.rgb, alpha);
}